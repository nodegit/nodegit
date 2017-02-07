NAN_METHOD(GitRemote::ReferenceList)
{
  if (info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("RemoteCallbacks callbacks is required.");
  }
  if (info.Length() == 1 || !info[1]->IsObject()) {
    return Nan::ThrowError("ProxyOptions proxy_opts is required.");
  }

  if (info.Length() == 2 || !(Nan::To<bool>(info[2]).FromJust())) {
    return Nan::ThrowError("Array, String Object, or string custom_headers is required.");
  }
  if (info.Length() == 3 || !info[3]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  ReferenceListBaton* baton = new ReferenceListBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_remote_head*>;
  baton->remote = Nan::ObjectWrap::Unwrap<GitRemote>(info.This())->GetValue();
  baton->callbacks = Nan::ObjectWrap::Unwrap<GitRemoteCallbacks>(info[0]->ToObject())->GetValue();
  baton->proxy_opts = Nan::ObjectWrap::Unwrap<GitProxyOptions>(info[1]->ToObject())->GetValue();
  baton->custom_headers = StrArrayConverter::Convert(info[2]);

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[3]));
  ReferenceListWorker *worker = new ReferenceListWorker(baton, callback);
  worker->SaveToPersistent("remote", info.This());
  if (!info[1]->IsUndefined() && !info[0]->IsNull())
    worker->SaveToPersistent("callbacks", info[0]->ToObject());
  if (!info[2]->IsUndefined() && !info[1]->IsNull())
    worker->SaveToPersistent("proxy_opts", info[1]->ToObject());
  if (!info[3]->IsUndefined() && !info[2]->IsNull())
    worker->SaveToPersistent("custom_headers", info[2]->ToObject());
  Nan::AsyncQueueWorker(worker);
  return;
}

void GitRemote::ReferenceListWorker::Execute()
{
  giterr_clear();
  baton->error_code = git_remote_connect(
    baton->remote,
    GIT_DIRECTION_FETCH,
    baton->callbacks,
    baton->proxy_opts,
    baton->custom_headers
  );

  if (baton->error_code != GIT_OK) {
    baton->error = git_error_dup(giterr_last());
    delete baton->out;
    baton->out = NULL;
    return;
  }

  const git_remote_head **remote_heads;
  size_t num_remote_heads;
  baton->error_code = git_remote_ls(
    &remote_heads,
    &num_remote_heads,
    baton->remote
  );

  git_remote_disconnect(baton->remote);
  if (baton->error_code != GIT_OK) {
    baton->error = git_error_dup(giterr_last());
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
    callback->Call(2, argv);
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
    Local<v8::Object> err = Nan::Error("Method next has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv);
  }
  else
  {
    callback->Call(0, NULL);
  }
}
