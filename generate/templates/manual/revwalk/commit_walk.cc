#define SET_ON_OBJECT(obj, field, data) Nan::Set(obj, Nan::New(field).ToLocalChecked(), data)

v8::Local<v8::Object> signatureToJavascript(const git_signature *signature) {
  v8::Local<v8::Object> signatureObject = Nan::New<v8::Object>();
  SET_ON_OBJECT(signatureObject, "name", Nan::New(signature->name).ToLocalChecked());
  SET_ON_OBJECT(signatureObject, "email", Nan::New(signature->email).ToLocalChecked());
  SET_ON_OBJECT(signatureObject, "date", Nan::New<v8::Number>(signature->when.time * 1000));
  std::stringstream fullSignature;
  fullSignature << signature->name << " <" << signature << ">";
  SET_ON_OBJECT(signatureObject, "full", Nan::New(fullSignature.str()).ToLocalChecked());
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

  v8::Local<v8::Value> toJavascript() {
    if (!fetchSignature) {
      v8::Local<v8::Value> commitObject = GitCommit::New(
        commit,
        true,
        Nan::To<v8::Object>(GitRepository::New(
          git_commit_owner(commit),
          true
        )).ToLocalChecked()
      );
      commit = NULL;
      return commitObject;
    }

    v8::Local<v8::Object> commitModel = Nan::New<v8::Object>();
    SET_ON_OBJECT(commitModel, "sha", Nan::New(git_oid_tostr_s(git_commit_id(commit))).ToLocalChecked());
    SET_ON_OBJECT(commitModel, "message", Nan::New(git_commit_message(commit)).ToLocalChecked());
    SET_ON_OBJECT(commitModel, "author", signatureToJavascript(git_commit_author(commit)));
    SET_ON_OBJECT(commitModel, "committer", signatureToJavascript(git_commit_committer(commit)));

    size_t parentCount = parentIds.size();
    v8::Local<v8::Array> parents = Nan::New<v8::Array>(parentCount);
    for (size_t parentIndex = 0; parentIndex < parentCount; ++parentIndex) {
      Nan::Set(parents, Nan::New<v8::Number>(parentIndex), Nan::New(parentIds[parentIndex]).ToLocalChecked());
    }
    SET_ON_OBJECT(commitModel, "parents", parents);

    if (signature.size != 0 || signedData.size != 0) {
      v8::Local<v8::Object> gpgSignature = Nan::New<v8::Object>();
      if (signature.size != 0) {
        SET_ON_OBJECT(gpgSignature, "signature", Nan::New(signature.ptr).ToLocalChecked());
      } else {
        SET_ON_OBJECT(gpgSignature, "signature", Nan::Null());
      }

      if (signedData.size != 0) {
        SET_ON_OBJECT(gpgSignature, "signedData", Nan::New(signedData.ptr).ToLocalChecked());
      } else {
        SET_ON_OBJECT(gpgSignature, "signedData", Nan::Null());
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

NAN_METHOD(GitRevwalk::CommitWalk) {
  if (info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Max count is required and must be a number.");
  }

  if (info.Length() >= 3 && !info[1]->IsNull() && !info[1]->IsUndefined() && !info[1]->IsObject()) {
    return Nan::ThrowError("Options must be an object, null, or undefined.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  CommitWalkBaton* baton = new CommitWalkBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->max_count = Nan::To<unsigned int>(info[0]).FromJust();
  std::vector<CommitModel *> *out = new std::vector<CommitModel *>;
  out->reserve(baton->max_count);
  baton->out = static_cast<void *>(out);
  if (info.Length() == 3 && info[1]->IsObject()) {
    v8::Local<v8::Object> options = Nan::To<v8::Object>(info[1]).ToLocalChecked();
    v8::Local<v8::String> propName = Nan::New("returnPlainObjects").ToLocalChecked();
    if (Nan::Has(options, propName).FromJust()) {
      baton->returnPlainObjects = Nan::Get(options, propName).ToLocalChecked()->IsTrue();
    } else {
      baton->returnPlainObjects = false;
    }
  } else {
    baton->returnPlainObjects = false;
  }
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();
  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  CommitWalkWorker *worker = new CommitWalkWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("commitWalk", info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
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
  if (baton->out != NULL) {
    std::vector<CommitModel *> *out = static_cast<std::vector<CommitModel *> *>(baton->out);
    const unsigned int size = out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      CommitModel *commitModel = out->at(i);
      Nan::Set(
        result,
        Nan::New<Number>(i),
        commitModel->toJavascript()
      );
      delete commitModel;
    }

    delete out;

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  } else if (baton->error) {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  } else if (baton->error_code < 0) {
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Revwalk commitWalk has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.commitWalk").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  } else {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}
