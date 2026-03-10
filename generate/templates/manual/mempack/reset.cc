// Manual binding for git_mempack_reset, exposed as Mempack.prototype.reset()

NAN_METHOD(GitMempack::Reset) {
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  ResetBaton *baton = new ResetBaton();
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->backend = Nan::ObjectWrap::Unwrap<GitMempack>(info.This())->GetValue();

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ResetWorker *worker = new ResetWorker(baton, callback, cleanupHandles);

  worker->Reference<GitMempack>("backend", info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitMempack::ResetWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->backend);
  return lockMaster;
}

void GitMempack::ResetWorker::Execute() {
  git_error_clear();

  baton->error_code = git_mempack_reset(baton->backend);

  if (baton->error_code != GIT_OK && git_error_last() != NULL && git_error_last()->klass != GIT_ERROR_NONE) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitMempack::ResetWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton;
}

void GitMempack::ResetWorker::HandleOKCallback() {
  v8::Local<v8::Value> argv[1] = {Nan::Null()};
  callback->Call(1, argv, async_resource);

  delete baton;
}
