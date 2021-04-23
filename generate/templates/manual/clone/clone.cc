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

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  CloneBaton *baton = new CloneBaton();
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;

  if (info[2]->IsNull() || info[2]->IsUndefined()) {
    baton->options = nullptr;
  } else {
    auto conversionResult = ConfigurableGitCloneOptions::fromJavascript(nodegitContext, info[2]);
    if (!conversionResult.result) {
      return Nan::ThrowError(Nan::New(conversionResult.error).ToLocalChecked());
    }

    auto convertedObject = conversionResult.result;
    cleanupHandles["options"] = convertedObject;
    baton->options = convertedObject->GetValue();
  }

  baton->error_code = GIT_OK;
  baton->error = NULL;

  // start convert_from_v8 block
  const char *from_url = NULL;

  Nan::Utf8String url(Nan::To<v8::String>(info[0]).ToLocalChecked());
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

  Nan::Utf8String local_path(Nan::To<v8::String>(info[1]).ToLocalChecked());
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

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  CloneWorker *worker = new CloneWorker(baton, callback, cleanupHandles);

  worker->Reference("url", info[0]);
  worker->Reference("local_path", info[1]);

  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitClone::CloneWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(
    true,
    baton->url,
    baton->local_path,
    baton->options
  );
  return lockMaster;
}

void GitClone::CloneWorker::Execute() {
  git_error_clear();

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

void GitClone::CloneWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  git_repository_free(baton->out);

  free((void*)baton->url);
  free((void*)baton->local_path);

  delete baton;
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
        err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
      } else {
        err = Nan::To<v8::Object>(Nan::Error("Method clone has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("Clone.clone").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
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

      if (!callbackFired) {
        v8::Local<v8::Object> err =
            Nan::To<v8::Object>(Nan::Error("Method clone has thrown an error.")).ToLocalChecked();
        Nan::Set(err, Nan::New("errno").ToLocalChecked(),
                 Nan::New(baton->error_code));
        Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
                 Nan::New("Clone.clone").ToLocalChecked());
        v8::Local<v8::Value> argv[1] = {err};
        callback->Call(1, argv, async_resource);
      }
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  free((void*)baton->url);
  free((void*)baton->local_path);

  delete baton;
}
