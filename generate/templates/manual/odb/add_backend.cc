// Manual binding for git_odb_add_backend, exposed as Odb.prototype.addMempackBackend()
// The first argument (backend) must be a Mempack object wrapping a git_odb_backend*.

NAN_METHOD(GitOdb::AddMempackBackend) {
  if (info.Length() < 3) {
    return Nan::ThrowError("Backend, priority, and callback arguments are required.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  AddMempackBackendBaton *baton = new AddMempackBackendBaton();
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->odb = Nan::ObjectWrap::Unwrap<GitOdb>(info.This())->GetValue();

  // Validate and extract git_odb_backend* from the Mempack wrapper
  if (!info[0]->IsObject() || info[0]->IsNull() || info[0]->IsUndefined()) {
    delete baton;
    return Nan::ThrowTypeError("First argument must be a Mempack object.");
  }
  baton->backend = Nan::ObjectWrap::Unwrap<GitMempack>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->GetValue();

  // Priority is required and must be a number
  if (!info[1]->IsNumber()) {
    delete baton;
    return Nan::ThrowTypeError("Second argument (priority) must be a number.");
  }
  baton->priority = (int)Nan::To<int32_t>(info[1]).FromJust();

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  AddMempackBackendWorker *worker = new AddMempackBackendWorker(baton, callback, cleanupHandles);

  worker->Reference<GitOdb>("odb", info.This());
  worker->Reference("backend", info[0]);

  // Store a reference from the backend to the ODB to prevent the ODB from
  // being garbage-collected while the backend is alive. This is critical
  // because after git_odb_add_backend, libgit2 owns the backend pointer and
  // will free it when the ODB is freed. If the ODB is GC'd first, the
  // backend's pointer becomes dangling.
  Nan::Set(Nan::To<v8::Object>(info[0]).ToLocalChecked(),
           Nan::New("_parentOdb").ToLocalChecked(), info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitOdb::AddMempackBackendWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->odb, baton->backend);
  return lockMaster;
}

void GitOdb::AddMempackBackendWorker::Execute() {
  git_error_clear();

  baton->error_code = git_odb_add_backend(baton->odb, baton->backend, baton->priority);

  if (baton->error_code != GIT_OK && git_error_last() != NULL && git_error_last()->klass != GIT_ERROR_NONE) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitOdb::AddMempackBackendWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton;
}

void GitOdb::AddMempackBackendWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> argv[2] = {Nan::Null(), Nan::New(baton->error_code)};
    callback->Call(2, argv, async_resource);
  } else if (baton->error) {
    v8::Local<v8::Object> err;
    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method addMempackBackend has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
             Nan::New("Odb.addMempackBackend").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {err};
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
      free((void *)baton->error->message);
    free((void *)baton->error);
  } else if (baton->error_code < 0) {
    v8::Local<v8::Object> err =
        Nan::To<v8::Object>(Nan::Error("Method addMempackBackend has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(),
             Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
             Nan::New("Odb.addMempackBackend").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {err};
    callback->Call(1, argv, async_resource);
  } else {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}
