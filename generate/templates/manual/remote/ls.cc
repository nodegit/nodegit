NAN_METHOD(GitRemote::ReferenceList)
{
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  ReferenceListBaton* baton = new ReferenceListBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_remote_head*>;
  baton->remote = Nan::ObjectWrap::Unwrap<GitRemote>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ReferenceListWorker *worker = new ReferenceListWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRemote>("remote", info.This());
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
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
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      Nan::Set(result, Nan::New<Number>(i), GitRemoteHead::New(baton->out->at(i), true));
    }

    delete baton->out;

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else if (baton->error)
  {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }
  else if (baton->error_code < 0)
  {
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Reference List has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Remote.referenceList").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else
  {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}
