NAN_METHOD(GitRevwalk::FirstCommit)
{
  if (info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Timeout(ms) is required and must be a number.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FirstCommitBaton* baton = new FirstCommitBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->timeout = Nan::To<unsigned int>(info[0]).FromJust();
  baton->found = false;
  baton->out = NULL;
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  FirstCommitWorker *worker = new FirstCommitWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("firstCommit", info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitRevwalk::FirstCommitWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitRevwalk::FirstCommitWorker::Execute()
{
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  git_oid *nextCommit = (git_oid *)malloc(sizeof(git_oid));
  git_error_clear();
  while(true) {
    baton->error_code = git_revwalk_next(nextCommit, baton->walk);

    if (baton->error_code != GIT_OK)
    {
      // We couldn't get a commit out of the revwalk. It's either in
      // an error state or there aren't anymore commits in the revwalk.
      if (baton->error_code != GIT_ITEROVER) {
        free(nextCommit);
        baton->error = git_error_dup(git_error_last());
        baton->out = NULL;
      }
      else {
        baton->error_code = GIT_OK;
        baton->out = nextCommit;
        baton->found = true;
      }

      break;
    }

    if(baton->timeout > 0 &&
       std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > baton->timeout) {
      baton->error_code = GIT_OK;
      baton->out = nextCommit;
      baton->found = false;
      break;
    }

  }
}

void GitRevwalk::FirstCommitWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton;
}

void GitRevwalk::FirstCommitWorker::HandleOKCallback()
{
  if (baton->out != NULL)
  {
    const v8::Local<v8::Object> result = Nan::New<v8::Object>();
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    if (result->Set(context, Nan::New<v8::String>("commit").ToLocalChecked(), GitOid::New(baton->out, true)).IsJust() == false) {
      return Nan::ThrowError("Could not set property commit");
    }
    if (result->Set(context, Nan::New<v8::String>("found").ToLocalChecked(), Nan::New<v8::Boolean>(baton->found)).IsJust() == false) {
      return Nan::ThrowError("Could not set property found");
    }

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else
  {
    if (baton->error)
    {
      Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
      } else {
        err = Nan::To<v8::Object>(Nan::Error("Method firstCommit has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.firstCommit").ToLocalChecked());
      Local<v8::Value> argv[1] = {
        err
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
      bool callbackFired = false;
      if (!callbackErrorHandle.IsEmpty()) {
        v8::Local<v8::Value> maybeError = Nan::New(callbackErrorHandle);
        if (!maybeError->IsNull() && !maybeError->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {
            maybeError
          };
          callback->Call(1, argv, async_resource);
          callbackFired = true;
        }
      }

      if (!callbackFired)
      {
        Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method next has thrown an error.")).ToLocalChecked();
        Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.firstCommit").ToLocalChecked());
        Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv, async_resource);
      }
    }
    else
    {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton;
}
