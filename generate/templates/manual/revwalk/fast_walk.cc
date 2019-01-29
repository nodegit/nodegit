NAN_METHOD(GitRevwalk::FastWalk)
{
  if (info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Max count is required and must be a number.");
  }

  if (info.Length() == 1 || !info[1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FastWalkBaton* baton = new FastWalkBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->max_count = Nan::To<unsigned int>(info[0]).FromJust();
  baton->out = new std::vector<git_oid*>;
  baton->out->reserve(baton->max_count);
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));
  FastWalkWorker *worker = new FastWalkWorker(baton, callback);
  worker->SaveToPersistent("fastWalk", info.This());

  Nan::AsyncQueueWorker(worker);
  return;
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

void GitRevwalk::FastWalkWorker::HandleOKCallback()
{
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      Nan::Set(result, Nan::New<Number>(i), GitOid::New(baton->out->at(i), true));
    }

    delete baton->out;

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
        err = Nan::Error(baton->error->message)->ToObject();
      } else {
        err = Nan::Error("Method fastWalk has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.fastWalk").ToLocalChecked());
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
      std::queue< Local<v8::Value> > workerArguments;
      bool callbackFired = false;

      while(!workerArguments.empty())
      {
        Local<v8::Value> node = workerArguments.front();
        workerArguments.pop();

        if (
          !node->IsObject()
          || node->IsArray()
          || node->IsBooleanObject()
          || node->IsDate()
          || node->IsFunction()
          || node->IsNumberObject()
          || node->IsRegExp()
          || node->IsStringObject()
        )
        {
          continue;
        }

        Local<v8::Object> nodeObj = node->ToObject();
        Local<v8::Value> checkValue = GetPrivate(nodeObj, Nan::New("NodeGitPromiseError").ToLocalChecked());

        if (!checkValue.IsEmpty() && !checkValue->IsNull() && !checkValue->IsUndefined())
        {
          Local<v8::Value> argv[1] = {
            checkValue->ToObject()
          };
          callback->Call(1, argv, async_resource);
          callbackFired = true;
          break;
        }

        Local<v8::Array> properties = nodeObj->GetPropertyNames();
        for (unsigned int propIndex = 0; propIndex < properties->Length(); ++propIndex)
        {
          Local<v8::String> propName = properties->Get(propIndex)->ToString();
          Local<v8::Value> nodeToQueue = nodeObj->Get(propName);
          if (!nodeToQueue->IsUndefined())
          {
            workerArguments.push(nodeToQueue);
          }
        }
      }

      if (!callbackFired)
      {
        Local<v8::Object> err = Nan::Error("Method next has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.fastWalk").ToLocalChecked());
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
}
