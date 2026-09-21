Napi::Value GitRemote::ReferenceList(const Napi::CallbackInfo& info) {
  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  ReferenceListBaton* baton = new ReferenceListBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_remote_head*>;
  baton->remote = NodeGitWrapper<GitRemoteTraits>::Unwrap<GitRemote>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ReferenceListWorker *worker = new ReferenceListWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRemote>("remote", info.This().As<Napi::Object>());
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
}

nodegit::LockMaster GitRemote::ReferenceListWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->remote);
  return lockMaster;
}

void GitRemote::ReferenceListWorker::Execute()
{
  git_error_clear();

  const git_remote_head **remote_heads;
  size_t num_remote_heads;
  baton->error_code = git_remote_ls(
    &remote_heads,
    &num_remote_heads,
    baton->remote
  );

  if (baton->error_code != GIT_OK) {
    baton->error = git_error_dup(git_error_last());
    delete baton->out;
    baton->out = NULL;
    return;
  }

  baton->out->reserve(num_remote_heads);

  for (size_t head_index = 0; head_index < num_remote_heads; ++head_index) {
    git_remote_head *remote_head = git_remote_head_dup(remote_heads[head_index]);
    baton->out->push_back(remote_head);
  }
}

void GitRemote::ReferenceListWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton->out;

  delete baton;
}

void GitRemote::ReferenceListWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      result.Set(i, GitRemoteHead::New(baton->out->at(i), true));
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
    Napi::Object err = Napi::Error::New(env, "Reference List has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Remote.referenceList"));
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
