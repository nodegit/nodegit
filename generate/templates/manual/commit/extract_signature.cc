NAN_METHOD(GitCommit::ExtractSignature)
{
  if (info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("Repository repo is required.");
  }

  if (info.Length() == 1 || (!info[1]->IsObject() && !info[1]->IsString())) {
    return Nan::ThrowError("Oid commit_id is required.");
  }

  if (info.Length() >= 4 && !info[2]->IsString() && !info[2]->IsUndefined() && !info[2]->IsNull()) {
    return Nan::ThrowError("String signature_field must be a string or undefined/null.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  ExtractSignatureBaton* baton = new ExtractSignatureBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->signature = GIT_BUF_INIT_CONST(NULL, 0);
  baton->signed_data = GIT_BUF_INIT_CONST(NULL, 0);
  baton->repo = Nan::ObjectWrap::Unwrap<GitRepository>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->GetValue();

  // baton->commit_id
  if (info[1]->IsString()) {
    Nan::Utf8String oidString(Nan::To<v8::String>(info[1]).ToLocalChecked());
    baton->commit_id = (git_oid *)malloc(sizeof(git_oid));
    baton->commit_idNeedsFree = true;
    string str = string(*oidString);
    if (git_oid_fromstr(baton->commit_id, str.c_str()) != GIT_OK) {
      free(baton->commit_id);

      if (git_error_last()) {
        return Nan::ThrowError(git_error_last()->message);
      } else {
        return Nan::ThrowError("Unknown Error");
      }
    }
  } else {
    baton->commit_id = Nan::ObjectWrap::Unwrap<GitOid>(Nan::To<v8::Object>(info[1]).ToLocalChecked())->GetValue();
    baton->commit_idNeedsFree = false;
  }

  // baton->field
  if (info[2]->IsString()) {
    Nan::Utf8String field(Nan::To<v8::String>(info[2]).ToLocalChecked());
    baton->field = (char *)malloc(field.length() + 1);
    memcpy((void *)baton->field, *field, field.length());
    baton->field[field.length()] = 0;
  } else {
    baton->field = NULL;
  }

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));

  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ExtractSignatureWorker *worker = new ExtractSignatureWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info[0]);
  worker->Reference<GitOid>("commit_id", info[1]);
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitCommit::ExtractSignatureWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->repo);
  return lockMaster;
}

void GitCommit::ExtractSignatureWorker::Execute()
{
  git_error_clear();

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

void GitCommit::ExtractSignatureWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  git_buf_dispose(&baton->signature);
  git_buf_dispose(&baton->signed_data);

  if (baton->commit_idNeedsFree) {
    free(baton->commit_id);
  }

  free(baton->field);

  delete baton;
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
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Extract Signature has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Commit.extractSignature").ToLocalChecked());
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

  if (baton->commit_idNeedsFree) {
    free(baton->commit_id);
  }

  free(baton->field);

  delete baton;
}
