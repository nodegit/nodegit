Napi::Value GitCommit::ExtractSignature(const Napi::CallbackInfo& info)
{
  if (info.Length() == 0 || !info[0].IsObject()) {
    Napi::Error::New(info.Env(), "Repository repo is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (info.Length() == 1 || (!info[1].IsObject() && !info[1].IsString())) {
    Napi::Error::New(info.Env(), "Oid commit_id is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (info.Length() >= 4 && !info[2].IsString() && !info[2].IsUndefined() && !info[2].IsNull()) {
    Napi::Error::New(info.Env(), "String signature_field must be a string or undefined/null.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  ExtractSignatureBaton* baton = new ExtractSignatureBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->signature = GIT_BUF_INIT_CONST(NULL, 0);
  baton->signed_data = GIT_BUF_INIT_CONST(NULL, 0);
  baton->repo = NodeGitWrapper<GitRepositoryTraits>::Unwrap<GitRepository>(info[0].As<Napi::Object>())->GetValue();

  // baton->commit_id
  if (info[1].IsString()) {
    std::string oidStr = info[1].As<Napi::String>().Utf8Value();
    baton->commit_id = (git_oid *)malloc(sizeof(git_oid));
    if (git_oid_fromstr(baton->commit_id, (const char *)strdup(oidStr.c_str())) != GIT_OK) {
      free(baton->commit_id);

      if (git_error_last()->klass != GIT_ERROR_NONE) {
        Napi::Error::New(info.Env(), git_error_last()->message).ThrowAsJavaScriptException();
        return info.Env().Undefined();
      } else {
        Napi::Error::New(info.Env(), "Unknown Error").ThrowAsJavaScriptException();
        return info.Env().Undefined();
      }
    }
  } else {
    baton->commit_id = NodeGitWrapper<GitOidTraits>::Unwrap<GitOid>(info[1].As<Napi::Object>())->GetValue();
  }

  // baton->field
  if (info[2].IsString()) {
    std::string fieldStr = info[2].As<Napi::String>().Utf8Value();
    baton->field = (char *)malloc(fieldStr.length() + 1);
    memcpy((void *)baton->field, fieldStr.c_str(), fieldStr.length());
    baton->field[fieldStr.length()] = 0;
  } else {
    baton->field = NULL;
  }

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));

  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ExtractSignatureWorker *worker = new ExtractSignatureWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info[0]);
  worker->Reference<GitOid>("commit_id", info[1]);
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
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

  if (baton->error_code != GIT_OK) {
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

  free(baton->field);

  delete baton;
}

void GitCommit::ExtractSignatureWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->error_code == GIT_OK)
  {
    Napi::Object result = Napi::Object::New(env);
    result.Set(
      "signature",
      Napi::String::New(env, baton->signature.ptr, baton->signature.size)
    );
    result.Set(
      "signedData",
      Napi::String::New(env, baton->signed_data.ptr, baton->signed_data.size)
    );

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
    Napi::Object err = Napi::Error::New(env, "Extract Signature has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Commit.extractSignature"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  }
  else
  {
    CallCallback(nullptr, 0);
  }

  git_buf_dispose(&baton->signature);
  git_buf_dispose(&baton->signed_data);

  if (baton->field != NULL) {
    free((void *)baton->field);
  }

  delete baton;
}
