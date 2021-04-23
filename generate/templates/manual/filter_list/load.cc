// NOTE you may need to occasionally rebuild this method by calling the generators
// if major changes are made to the templates / generator.

// git_filter_list_load has a more complex ownership pattern than is currently available
// in the generator. This is because it not only has to get the repo as an owner,
// but it also needs to discover which custom filters, a git_filter, it is bound to, if any.
// We must enforce that the custom filters are not freed before a git_filter_list is freed,
// but a git_filter_list also has pointers to the repo in it.

// TODO In the future, it would be awesome if we could instead of writing a manual method like this, provide
// custom ownership methods that can be injected into the HandleOKCallback.

/*
 * @param Repository repo
 * @param Blob blob
 * @param String path
 * @param Number mode
 * @param Number flags
 * @param FilterList callback
 */
NAN_METHOD(GitFilterList::Load) {
  if (info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("Repository repo is required.");
  }

  if (info.Length() == 2 || !info[2]->IsString()) {
    return Nan::ThrowError("String path is required.");
  }

  if (info.Length() == 3 || !info[3]->IsNumber()) {
    return Nan::ThrowError("Number mode is required.");
  }

  if (info.Length() == 4 || !info[4]->IsNumber()) {
    return Nan::ThrowError("Number flags is required.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  LoadBaton *baton = new LoadBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;

  // start convert_from_v8 block
  git_repository *from_repo = NULL;
  from_repo =
      Nan::ObjectWrap::Unwrap<GitRepository>(Nan::To<v8::Object>(info[0]).ToLocalChecked())->GetValue();
  // end convert_from_v8 block
  baton->repo = from_repo;
  // start convert_from_v8 block
  git_blob *from_blob = NULL;
  if (info[1]->IsObject()) {
    from_blob =
        Nan::ObjectWrap::Unwrap<GitBlob>(Nan::To<v8::Object>(info[1]).ToLocalChecked())->GetValue();
  } else {
    from_blob = 0;
  }
  // end convert_from_v8 block
  baton->blob = from_blob;
  // start convert_from_v8 block
  const char *from_path = NULL;

  Nan::Utf8String path(Nan::To<v8::String>(info[2]).ToLocalChecked());
  // malloc with one extra byte so we can add the terminating null character
  // C-strings expect:
  from_path = (const char *)malloc(path.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead
  // of strdup or strcpy because nulls in the middle of strings are valid coming
  // from nodejs):
  memcpy((void *)from_path, *path, path.length());
  // ensure the final byte of our new string is null, extra casts added to
  // ensure compatibility with various C types used in the nodejs binding
  // generation:
  memset((void *)(((char *)from_path) + path.length()), 0, 1);
  // end convert_from_v8 block
  baton->path = from_path;
  // start convert_from_v8 block
  git_filter_mode_t from_mode;
  from_mode = (git_filter_mode_t)(int)info[3].As<v8::Number>()->Value();
  // end convert_from_v8 block
  baton->mode = from_mode;
  // start convert_from_v8 block
  uint32_t from_flags;
  from_flags = (uint32_t)info[4].As<v8::Number>()->Value();
  // end convert_from_v8 block
  baton->flags = from_flags;

  Nan::Callback *callback =
      new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  LoadWorker *worker = new LoadWorker(baton, callback, cleanupHandles);

  worker->Reference<GitRepository>("repo", info[0]);
  worker->Reference<GitBlob>("blob", info[1]);

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitFilterList::LoadWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(
    true,
    baton->repo,
    baton->blob,
    baton->path
  );
  return lockMaster;
}

void GitFilterList::LoadWorker::Execute() {
  git_error_clear();

  int result = git_filter_list_load(&baton->filters, baton->repo, baton->blob,
                                    baton->path, baton->mode, baton->flags);

  baton->error_code = result;

  if (result != GIT_OK && git_error_last() != NULL) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitFilterList::LoadWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  git_filter_list_free(baton->filters);

  free((void *)baton->path);

  delete baton;
}

void GitFilterList::LoadWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> to;
    // start convert_to_v8 block

    if (baton->filters != NULL) {
      // GitFilterList baton->filters
      v8::Local<v8::Array> owners = Nan::New<Array>(0);
      nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();

      Nan::Set(
        owners,
        Nan::New<Number>(0),
        Nan::To<v8::Object>(this->GetFromPersistent("repo")).ToLocalChecked()
      );

      to = GitFilterList::New(baton->filters, true, Nan::To<v8::Object>(owners).ToLocalChecked());
      auto filterListWrapper = Nan::ObjectWrap::Unwrap<GitFilterList>(to.As<v8::Object>());
      auto filterRegistryCleanupHandles = static_pointer_cast<nodegit::FilterRegistryCleanupHandles>(nodegit::Context::GetCurrentContext()->GetCleanupHandle("filterRegistry"));
      std::for_each(
        filterRegistryCleanupHandles->registeredFilters.begin(),
        filterRegistryCleanupHandles->registeredFilters.end(),
        [this, &filterListWrapper](std::pair<std::string, std::shared_ptr<nodegit::CleanupHandle>> filterCleanupHandle) {
          if (git_filter_list_contains(baton->filters, filterCleanupHandle.first.c_str())) {
            filterListWrapper->SaveCleanupHandle(filterCleanupHandle.second);
          }
        }
      );
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
        err = Nan::To<v8::Object>(Nan::Error("Method load has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterList.load").ToLocalChecked());
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
            Nan::To<v8::Object>(Nan::Error("Method load has thrown an error.")).ToLocalChecked();
        Nan::Set(err, Nan::New("errno").ToLocalChecked(),
                 Nan::New(baton->error_code));
        Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
                 Nan::New("FilterList.load").ToLocalChecked());
        v8::Local<v8::Value> argv[1] = {err};
        callback->Call(1, argv, async_resource);
      }
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  free((void *)baton->path);

  delete baton;
}
