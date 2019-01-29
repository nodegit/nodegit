NAN_METHOD(GitCommit::ExtractSignature)
{
  if (info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("Repository repo is required.");
  }

  if (info.Length() == 1 || (!info[1]->IsObject() && !info[1]->IsString())) {
    return Nan::ThrowError("Oid commit_id is required.");
  }

  if (info.Length() == 2 || (info.Length() == 3 && !info[2]->IsFunction())) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  if (info.Length() >= 4) {
    if (!info[2]->IsString() && !info[2]->IsUndefined() && !info[2]->IsNull()) {
      return Nan::ThrowError("String signature_field must be a string or undefined/null.");
    }

    if (!info[3]->IsFunction()) {
      return Nan::ThrowError("Callback is required and must be a Function.");
    }
  }

  ExtractSignatureBaton* baton = new ExtractSignatureBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->signature = GIT_BUF_INIT_CONST(NULL, 0);
  baton->signed_data = GIT_BUF_INIT_CONST(NULL, 0);
  baton->repo = Nan::ObjectWrap::Unwrap<GitRepository>(info[0]->ToObject())->GetValue();

  // baton->commit_id
  if (info[1]->IsString()) {
    String::Utf8Value oidString(info[1]->ToString());
    baton->commit_id = (git_oid *)malloc(sizeof(git_oid));
    if (git_oid_fromstr(baton->commit_id, (const char *)strdup(*oidString)) != GIT_OK) {
      free(baton->commit_id);

      if (git_error_last()) {
        return Nan::ThrowError(git_error_last()->message);
      } else {
        return Nan::ThrowError("Unknown Error");
      }
    }
  } else {
    baton->commit_id = Nan::ObjectWrap::Unwrap<GitOid>(info[1]->ToObject())->GetValue();
  }

  // baton->field
  if (info[2]->IsString()) {
    String::Utf8Value field(info[2]->ToString());
    baton->field = (char *)malloc(field.length() + 1);
    memcpy((void *)baton->field, *field, field.length());
    baton->field[field.length()] = 0;
  } else {
    baton->field = NULL;
  }

  Nan::Callback *callback;
  if (info[2]->IsFunction()) {
    callback = new Nan::Callback(Local<Function>::Cast(info[2]));
  } else {
    callback = new Nan::Callback(Local<Function>::Cast(info[3]));
  }

  ExtractSignatureWorker *worker = new ExtractSignatureWorker(baton, callback);
  worker->SaveToPersistent("repo", info[0]->ToObject());
  worker->SaveToPersistent("commit_id", info[1]->ToObject());
  Nan::AsyncQueueWorker(worker);
  return;
}

void GitCommit::ExtractSignatureWorker::Execute()
{
  git_error_clear();

  {
    LockMaster lockMaster(
      /*asyncAction: */true,
      baton->repo
    );

    baton->error_code = git_commit_extract_signature(
      &baton->signature,
      &baton->signed_data,
      baton->repo,
      baton->commit_id,
      (const char *)baton->field
    );

    if (baton->error_code != GIT_OK && git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
}

void GitCommit::ExtractSignatureWorker::HandleOKCallback()
{
  if (baton->error_code == GIT_OK)
  {
    Local<v8::Object> result = Nan::New<Object>();
    Nan::Set(
      result,
      Nan::New("signature").ToLocalChecked(),
      Nan::New<String>(baton->signature.ptr, baton->signature.size).ToLocalChecked()
    );
    Nan::Set(
      result,
      Nan::New("signedData").ToLocalChecked(),
      Nan::New<String>(baton->signed_data.ptr, baton->signed_data.size).ToLocalChecked()
    );

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
    Local<v8::Object> err = Nan::Error("Extract Signature has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Commit.extractSignature").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else
  {
    callback->Call(0, NULL, async_resource);
  }

  git_buf_dispose(&baton->signature);
  git_buf_dispose(&baton->signed_data);

  if (baton->field != NULL) {
    free((void *)baton->field);
  }

  delete baton;
}
