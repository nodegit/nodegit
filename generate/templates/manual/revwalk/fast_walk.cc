Napi::Value GitRevwalk::FastWalk(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();
  if (info.Length() == 0 || !info[0].IsNumber()) {
    Napi::Error::New(env, "Max count is required and must be a number.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  FastWalkBaton* baton = new FastWalkBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->max_count = info[0].As<Napi::Number>().Uint32Value();
  baton->out = new std::vector<git_oid*>;
  baton->out->reserve(baton->max_count);
  baton->walk = NodeGitWrapper<GitRevwalkTraits>::Unwrap<GitRevwalk>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  FastWalkWorker *worker = new FastWalkWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("fastWalk", info.This().As<Napi::Object>());

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return env.Undefined();
}

nodegit::LockMaster GitRevwalk::FastWalkWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitRevwalk::FastWalkWorker::Execute()
{
  for (int i = 0; i < baton->max_count; i++)
  {
    git_oid *nextCommit = (git_oid *)malloc(sizeof(git_oid));
    git_error_clear();
    baton->error_code = git_revwalk_next(nextCommit, baton->walk);

    if (baton->error_code != GIT_OK)
    {
      // We couldn't get a commit out of the revwalk. It's either in
      // an error state or there aren't anymore commits in the revwalk.
      free(nextCommit);

      if (baton->error_code != GIT_ITEROVER) {
        baton->error = git_error_dup(git_error_last());

        while(!baton->out->empty())
        {
          // part of me wants to #define shoot free so we can take the
          // baton out back and shoot the oids
          git_oid *oidToFree = baton->out->back();
          free(oidToFree);
          baton->out->pop_back();
        }

        delete baton->out;

        baton->out = NULL;
      }
      else {
        baton->error_code = GIT_OK;
      }

      break;
    }

    baton->out->push_back(nextCommit);
  }
}

void GitRevwalk::FastWalkWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  while(!baton->out->empty()) {
    free(baton->out->back());
    baton->out->pop_back();
  }

  delete baton->out;

  delete baton;
}

void GitRevwalk::FastWalkWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      result.Set(i, GitOid::New(baton->out->at(i), true));
    }

    delete baton->out;

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);
  }
  else
  {
    if (baton->error)
    {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method fastWalk has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "Revwalk.fastWalk"));
      Napi::Value argv[1] = {
        err
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
      bool callbackFired = false;
      if (!callbackErrorHandle.IsEmpty()) {
        Napi::Value maybeError = callbackErrorHandle.Value();
        if (!maybeError.IsNull() && !maybeError.IsUndefined()) {
          Napi::Value argv[1] = {
            maybeError
          };
          CallCallback(argv, 1);
          callbackFired = true;
        }
      }

      if (!callbackFired)
      {
        Napi::Object err = Napi::Error::New(env, "Method next has thrown an error.").Value();
        err.Set("errno", Napi::Number::New(env, baton->error_code));
        err.Set("errorFunction", Napi::String::New(env, "Revwalk.fastWalk"));
        Napi::Value argv[1] = {
          err
        };
        CallCallback(argv, 1);
      }
    }
    else
    {
      CallCallback(nullptr, 0);
    }
  }

  delete baton;
}
