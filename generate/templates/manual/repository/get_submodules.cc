Napi::Value GitRepository::GetSubmodules(const Napi::CallbackInfo& info) {
  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  GetSubmodulesBaton* baton = new GetSubmodulesBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_submodule *>;
  baton->repo = NodeGitWrapper<GitRepositoryTraits>::Unwrap<GitRepository>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  GetSubmodulesWorker *worker = new GetSubmodulesWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info.This().As<Napi::Object>());
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
}

struct submodule_foreach_payload {
  git_repository *repo;
  std::vector<git_submodule *> *out;
};

int foreachSubmoduleCB(git_submodule *submodule, const char *name, void *void_payload) {
  submodule_foreach_payload *payload = (submodule_foreach_payload *)void_payload;
  git_submodule *out;

  int result = git_submodule_lookup(&out, payload->repo, name);
  if (result == GIT_OK) {
    payload->out->push_back(out);
  }

  return result;
}

nodegit::LockMaster GitRepository::GetSubmodulesWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->repo);
  return lockMaster;
}

void GitRepository::GetSubmodulesWorker::Execute()
{
  giterr_clear();

  submodule_foreach_payload payload { baton->repo, baton->out };
  baton->error_code = git_submodule_foreach(baton->repo, foreachSubmoduleCB, (void *)&payload);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }

    while (baton->out->size()) {
      git_submodule_free(baton->out->back());
      baton->out->pop_back();
    }
    delete baton->out;
    baton->out = NULL;
  }
}

void GitRepository::GetSubmodulesWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  while (baton->out->size()) {
    git_submodule_free(baton->out->back());
    baton->out->pop_back();
  }

  delete baton->out;

  delete baton;
}

void GitRepository::GetSubmodulesWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      git_submodule *submodule = baton->out->at(i);
      result.Set(
        i,
        GitSubmodule::New(
          submodule,
          true,
          GitRepository::New(git_submodule_owner(submodule), true).As<Napi::Object>()
        )
      );
    }

    delete baton->out;

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);
  }
  else if (baton->error)
  {
    Napi::Value argv[1] = {
      Napi::Error::New(env, baton->error->message).Value()
    };
    CallCallback(argv, 1);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }
  else if (baton->error_code < 0)
  {
    Napi::Object err = Napi::Error::New(env, "Repository getSubmodules has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Repository.getSubmodules"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  }
  else
  {
    CallCallback(nullptr, 0);
  }

  delete baton;
}
