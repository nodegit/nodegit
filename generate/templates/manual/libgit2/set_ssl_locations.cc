NAN_METHOD(GitLibgit2::SetSslLocations)
{
  if (info.Length() == 0 || !info[0]->IsString() || !info[0]->IsUndefined() || !info[0]->IsNull()) {
    return Nan::ThrowError("CAInfo must be a string or null/undefined.");
  }

  if (info.Length() == 1 || !info[1]->IsString() || !info[1]->IsUndefined() || !info[1]->IsNull()) {
    return Nan::ThrowError("CAPath must be a string or null/undefined.");
  }

  if (!info[2]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  SetSslLocationsBaton* baton = new SetSslLocationsBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->ca_info = NULL;
  baton->ca_path = NULL;

  if (info[0]->IsString()) {
    baton->ca_info = info[0]->ToString();
  }

  if (info[1]->IsString()) {
    baton->ca_path = info[1]->ToString();
  }

  Nan::Callback *callback;
  callback = new Nan::Callback(Local<Function>::Cast(info[2]));

  SetSslLocationsWorker *worker = new SetSslLocationsWorker(baton, callback);
  Nan::AsyncQueueWorker(worker);
  return;
}

void GitLibgit2::SetSslLocationsWorker::Execute()
{
  giterr_clear();

  {
    LockMaster lockMaster(true, true);

    baton->error_code = git_libgit2_opts(
      GIT_OPT_SET_SSL_CERT_LOCATIONS,
      baton->ca_info,
      baton->ca_path
    );

    if (baton->error_code != GIT_OK && giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
  }
}

void GitLibgit2::SetSslLocationsWorker::HandleOKCallback()
{
  if (baton->error_code == GIT_OK)
  {
    Local<v8::Value> argv[1] = {
      Nan::Null()
    };
    callback->Call(1, argv);
  }
  else if (baton->error)
  {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
    };
    callback->Call(1, argv);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }
  else if (baton->error_code < 0)
  {
    Local<v8::Object> err = Nan::Error("Set SSL Locations has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Libgit2Opts.setSslLocations").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv);
  }
  else
  {
    callback->Call(0, NULL);
  }

  delete baton;
}
