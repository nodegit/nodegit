// Manual binding for git_mempack_new, exposed as Mempack.create()

NAN_METHOD(GitMempack::Create) {
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  CreateBaton *baton = new CreateBaton();
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = NULL;

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  CreateWorker *worker = new CreateWorker(baton, callback, cleanupHandles);

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitMempack::CreateWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitMempack::CreateWorker::Execute() {
  git_error_clear();

  baton->error_code = git_mempack_new(&baton->out);

  if (baton->error_code != GIT_OK && git_error_last() != NULL && git_error_last()->klass != GIT_ERROR_NONE) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitMempack::CreateWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton;
}

void GitMempack::CreateWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    if (baton->out == NULL) {
      // This should never happen if error_code == GIT_OK
      v8::Local<v8::Value> argv[1] = {Nan::Error("Mempack creation returned OK but produced no backend.")};
      callback->Call(1, argv, async_resource);
      delete baton;
      return;
    }

    v8::Local<v8::Value> to = GitMempack::New(baton->out, false);
    v8::Local<v8::Value> argv[2] = {Nan::Null(), to};
    callback->Call(2, argv, async_resource);
  } else if (baton->error) {
    v8::Local<v8::Object> err;
    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method create has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
             Nan::New("Mempack.create").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {err};
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
      free((void *)baton->error->message);
    free((void *)baton->error);
  } else if (baton->error_code < 0) {
    v8::Local<v8::Object> err =
        Nan::To<v8::Object>(Nan::Error("Method create has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(),
             Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
             Nan::New("Mempack.create").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {err};
    callback->Call(1, argv, async_resource);
  } else {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}
