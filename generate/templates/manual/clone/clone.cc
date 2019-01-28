// NOTE you may need to occasionally rebuild this method by calling the generators
// if major changes are made to the templates / generator.

// Due to some file locking issues, we have the need to free a repository after it's cloned.
// We do not expose free functions to javascript, and so, we've moved the implementation of
// cloning, freeing the repo, and opening the repo into a custom template.

/*
 * @param String url
 * @param String local_path
 * @param CloneOptions options
 * @param Repository callback
 */
NAN_METHOD(GitClone::Clone) {

  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("String url is required.");
  }

  if (info.Length() == 1 || !info[1]->IsString()) {
    return Nan::ThrowError("String local_path is required.");
  }

  if (info.Length() == 3 || !info[3]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  CloneBaton *baton = new CloneBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;

  // start convert_from_v8 block
  const char *from_url = NULL;

  String::Utf8Value url(info[0]->ToString());
  // malloc with one extra byte so we can add the terminating null character
  // C-strings expect:
  from_url = (const char *)malloc(url.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead
  // of strdup or strcpy because nulls in the middle of strings are valid coming
  // from nodejs):
  memcpy((void *)from_url, *url, url.length());
  // ensure the final byte of our new string is null, extra casts added to
  // ensure compatibility with various C types used in the nodejs binding
  // generation:
  memset((void *)(((char *)from_url) + url.length()), 0, 1);
  // end convert_from_v8 block
  baton->url = from_url;
  // start convert_from_v8 block
  const char *from_local_path = NULL;

  String::Utf8Value local_path(info[1]->ToString());
  // malloc with one extra byte so we can add the terminating null character
  // C-strings expect:
  from_local_path = (const char *)malloc(local_path.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead
  // of strdup or strcpy because nulls in the middle of strings are valid coming
  // from nodejs):
  memcpy((void *)from_local_path, *local_path, local_path.length());
  // ensure the final byte of our new string is null, extra casts added to
  // ensure compatibility with various C types used in the nodejs binding
  // generation:
  memset((void *)(((char *)from_local_path) + local_path.length()), 0, 1);
  // end convert_from_v8 block
  baton->local_path = from_local_path;
  // start convert_from_v8 block
  const git_clone_options *from_options = NULL;
  if (info[2]->IsObject()) {
    from_options = Nan::ObjectWrap::Unwrap<GitCloneOptions>(info[2]->ToObject())
                       ->GetValue();
  } else {
    from_options = 0;
  }
  // end convert_from_v8 block
  baton->options = from_options;

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[3]));
  CloneWorker *worker = new CloneWorker(baton, callback);

  if (!info[0]->IsUndefined() && !info[0]->IsNull())
    worker->SaveToPersistent("url", info[0]->ToObject());
  if (!info[1]->IsUndefined() && !info[1]->IsNull())
    worker->SaveToPersistent("local_path", info[1]->ToObject());
  if (!info[2]->IsUndefined() && !info[2]->IsNull())
    worker->SaveToPersistent("options", info[2]->ToObject());

  AsyncLibgit2QueueWorker(worker);
  return;
}

void GitClone::CloneWorker::Execute() {
  git_error_clear();

  {
    LockMaster lockMaster(
        /*asyncAction: */ true, baton->url, baton->local_path, baton->options);

    git_repository *repo;
    int result =
        git_clone(&repo, baton->url, baton->local_path, baton->options);

    if (result == GIT_OK) {
      // This is required to clean up after the clone to avoid file locking
      // issues in Windows and potentially other issues we don't know about.
      git_repository_free(repo);

      // We want to provide a valid repository object, so reopen the repository
      // after clone and cleanup.
      result = git_repository_open(&baton->out, baton->local_path);
    }

    baton->error_code = result;

    if (result != GIT_OK && git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
}

void GitClone::CloneWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> to;
    // start convert_to_v8 block

    if (baton->out != NULL) {
      // GitRepository baton->out
      to = GitRepository::New(baton->out, true);
    } else {
      to = Nan::Null();
    }

    // end convert_to_v8 block
    v8::Local<v8::Value> result = to;

    v8::Local<v8::Value> argv[2] = {Nan::Null(), result};
    callback->Call(2, argv, async_resource);
  } else {
    if (baton->error) {
      v8::Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::Error(baton->error->message)->ToObject();
      } else {
        err = Nan::Error("Method clone has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      err->Set(Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("Clone.clone").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      std::queue<v8::Local<v8::Value>> workerArguments;
      workerArguments.push(GetFromPersistent("url"));
      workerArguments.push(GetFromPersistent("local_path"));
      workerArguments.push(GetFromPersistent("options"));
      bool callbackFired = false;
      while (!workerArguments.empty()) {
        v8::Local<v8::Value> node = workerArguments.front();
        workerArguments.pop();

        if (!node->IsObject() || node->IsArray() || node->IsBooleanObject() ||
            node->IsDate() || node->IsFunction() || node->IsNumberObject() ||
            node->IsRegExp() || node->IsStringObject()) {
          continue;
        }

        v8::Local<v8::Object> nodeObj = node->ToObject();
        v8::Local<v8::Value> checkValue = GetPrivate(
            nodeObj, Nan::New("NodeGitPromiseError").ToLocalChecked());

        if (!checkValue.IsEmpty() && !checkValue->IsNull() &&
            !checkValue->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {checkValue->ToObject()};
          callback->Call(1, argv, async_resource);
          callbackFired = true;
          break;
        }

        v8::Local<v8::Array> properties = nodeObj->GetPropertyNames();
        for (unsigned int propIndex = 0; propIndex < properties->Length();
             ++propIndex) {
          v8::Local<v8::String> propName =
              properties->Get(propIndex)->ToString();
          v8::Local<v8::Value> nodeToQueue = nodeObj->Get(propName);
          if (!nodeToQueue->IsUndefined()) {
            workerArguments.push(nodeToQueue);
          }
        }
      }

      if (!callbackFired) {
        v8::Local<v8::Object> err =
            Nan::Error("Method clone has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(),
                 Nan::New(baton->error_code));
        err->Set(Nan::New("errorFunction").ToLocalChecked(),
                 Nan::New("Clone.clone").ToLocalChecked());
        v8::Local<v8::Value> argv[1] = {err};
        callback->Call(1, argv, async_resource);
      }
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton;
}
