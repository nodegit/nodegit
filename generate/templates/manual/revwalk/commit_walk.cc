#define SET_ON_OBJECT(obj, field, data) obj.Set(field, data)

Napi::Object signatureToJavascript(Napi::Env env, const git_signature *signature) {
  Napi::Object signatureObject = Napi::Object::New(env);
  SET_ON_OBJECT(signatureObject, "name", Napi::String::New(env, signature->name));
  SET_ON_OBJECT(signatureObject, "email", Napi::String::New(env, signature->email));
  SET_ON_OBJECT(signatureObject, "date", Napi::Number::New(env, signature->when.time * 1000));
  std::stringstream fullSignature;
  fullSignature << signature->name << " <" << signature << ">";
  SET_ON_OBJECT(signatureObject, "full", Napi::String::New(env, fullSignature.str()));
  return signatureObject;
}

#include <iostream>
class CommitModel {
public:
  CommitModel(git_commit *commit, bool fetchSignature):
    commit(commit),
    fetchSignature(fetchSignature),
    signature({ 0, 0, 0 }),
    signedData({ 0, 0, 0 })
  {
    if (fetchSignature) {
      const int error = git_commit_extract_signature(
        &signature,
        &signedData,
        git_commit_owner(commit),
        const_cast<git_oid *>(git_commit_id(commit)),
        NULL
      );
      if (error != GIT_ENOTFOUND) {
        assert(error == GIT_OK);
      }
    }

    const size_t parentCount = git_commit_parentcount(commit);
    parentIds.reserve(parentCount);
    for (size_t parentIndex = 0; parentIndex < parentCount; ++parentIndex) {
      parentIds.push_back(git_oid_tostr_s(git_commit_parent_id(commit, parentIndex)));
    }
  }

  CommitModel(const CommitModel &) = delete;
  CommitModel(CommitModel &&) = delete;
  CommitModel &operator=(const CommitModel &) = delete;
  CommitModel &operator=(CommitModel &&) = delete;

  Napi::Value toJavascript(Napi::Env env) {
    if (!fetchSignature) {
      Napi::Value commitObject = GitCommit::New(
        commit,
        true,
        GitRepository::New(
          git_commit_owner(commit),
          true
        ).As<Napi::Object>()
      );
      commit = NULL;
      return commitObject;
    }

    Napi::Object commitModel = Napi::Object::New(env);
    SET_ON_OBJECT(commitModel, "sha", Napi::String::New(env, git_oid_tostr_s(git_commit_id(commit))));
    SET_ON_OBJECT(commitModel, "message", Napi::String::New(env, git_commit_message(commit)));
    SET_ON_OBJECT(commitModel, "author", signatureToJavascript(env, git_commit_author(commit)));
    SET_ON_OBJECT(commitModel, "committer", signatureToJavascript(env, git_commit_committer(commit)));

    size_t parentCount = parentIds.size();
    Napi::Array parents = Napi::Array::New(env, parentCount);
    for (size_t parentIndex = 0; parentIndex < parentCount; ++parentIndex) {
      parents.Set(parentIndex, Napi::String::New(env, parentIds[parentIndex]));
    }
    SET_ON_OBJECT(commitModel, "parents", parents);

    if (signature.size != 0 || signedData.size != 0) {
      Napi::Object gpgSignature = Napi::Object::New(env);
      if (signature.size != 0) {
        SET_ON_OBJECT(gpgSignature, "signature", Napi::String::New(env, signature.ptr));
      } else {
        SET_ON_OBJECT(gpgSignature, "signature", env.Null());
      }

      if (signedData.size != 0) {
        SET_ON_OBJECT(gpgSignature, "signedData", Napi::String::New(env, signedData.ptr));
      } else {
        SET_ON_OBJECT(gpgSignature, "signedData", env.Null());
      }

      SET_ON_OBJECT(commitModel, "gpgSignature", gpgSignature);
    }

    return commitModel;
  }

  ~CommitModel() {
    git_buf_dispose(&signature);
    git_buf_dispose(&signedData);
    if (commit) {
      git_commit_free(commit);
    }
  }

private:
  git_commit *commit;
  bool fetchSignature;
  git_buf signature, signedData;
  std::vector<std::string> parentIds;
};

Napi::Value GitRevwalk::CommitWalk(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0 || !info[0].IsNumber()) {
    Napi::Error::New(env, "Max count is required and must be a number.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (info.Length() >= 3 && !info[1].IsNull() && !info[1].IsUndefined() && !info[1].IsObject()) {
    Napi::Error::New(env, "Options must be an object, null, or undefined.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  CommitWalkBaton* baton = new CommitWalkBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->max_count = info[0].As<Napi::Number>().Uint32Value();
  std::vector<CommitModel *> *out = new std::vector<CommitModel *>;
  out->reserve(baton->max_count);
  baton->out = static_cast<void *>(out);
  if (info.Length() == 3 && info[1].IsObject()) {
    Napi::Object options = info[1].As<Napi::Object>();
    if (options.Has("returnPlainObjects")) {
      Napi::Value propValue = options.Get("returnPlainObjects");
      baton->returnPlainObjects = propValue.IsBoolean() && propValue.As<Napi::Boolean>().Value();
    } else {
      baton->returnPlainObjects = false;
    }
  } else {
    baton->returnPlainObjects = false;
  }
  baton->walk = NodeGitWrapper<GitRevwalkTraits>::Unwrap<GitRevwalk>(info.This().As<Napi::Object>())->GetValue();
  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  CommitWalkWorker *worker = new CommitWalkWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("commitWalk", info.This().As<Napi::Object>());

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return env.Undefined();
}

nodegit::LockMaster GitRevwalk::CommitWalkWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitRevwalk::CommitWalkWorker::Execute() {
  giterr_clear();

  std::vector<CommitModel *> *out = static_cast<std::vector<CommitModel *> *>(baton->out);
  for (int i = 0; i < baton->max_count; i++) {
    git_oid next_commit_id;
    baton->error_code = git_revwalk_next(&next_commit_id, baton->walk);

    if (baton->error_code == GIT_ITEROVER) {
      baton->error_code = GIT_OK;
      return;
    }

    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      while (out->size()) {
        delete out->back();
        out->pop_back();
      }

      delete out;
      baton->out = NULL;

      return;
    }

    git_commit *commit;
    baton->error_code = git_commit_lookup(&commit, git_revwalk_repository(baton->walk), &next_commit_id);

    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      while (out->size()) {
        delete out->back();
        out->pop_back();
      }

      delete out;
      baton->out = NULL;

      return;
    }

    out->push_back(new CommitModel(commit, baton->returnPlainObjects));
  }
}

void GitRevwalk::CommitWalkWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  auto out = static_cast<std::vector<CommitModel *> *>(baton->out);
  while (out->size()) {
    delete out->back();
    out->pop_back();
  }

  delete out;

  delete baton;
}

void GitRevwalk::CommitWalkWorker::HandleOKCallback() {
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL) {
    std::vector<CommitModel *> *out = static_cast<std::vector<CommitModel *> *>(baton->out);
    const unsigned int size = out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      CommitModel *commitModel = out->at(i);
      result.Set(i, commitModel->toJavascript(env));
      delete commitModel;
    }

    delete out;

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);
  } else if (baton->error) {
    Napi::Value argv[1] = {
      Napi::Error::New(env, baton->error->message).Value()
    };
    CallCallback(argv, 1);
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  } else if (baton->error_code < 0) {
    Napi::Object err = Napi::Error::New(env, "Revwalk commitWalk has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Revwalk.commitWalk"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  } else {
    CallCallback(nullptr, 0);
  }

  delete baton;
}
