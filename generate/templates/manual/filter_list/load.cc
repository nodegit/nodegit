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

  if (info.Length() == 5 || !info[5]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  LoadBaton *baton = new LoadBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;

  // start convert_from_v8 block
  git_repository *from_repo = NULL;
  from_repo =
      Nan::ObjectWrap::Unwrap<GitRepository>(info[0]->ToObject())->GetValue();
  // end convert_from_v8 block
  baton->repo = from_repo;
  // start convert_from_v8 block
  git_blob *from_blob = NULL;
  if (info[1]->IsObject()) {
    from_blob =
        Nan::ObjectWrap::Unwrap<GitBlob>(info[1]->ToObject())->GetValue();
  } else {
    from_blob = 0;
  }
  // end convert_from_v8 block
  baton->blob = from_blob;
  // start convert_from_v8 block
  const char *from_path = NULL;

  String::Utf8Value path(info[2]->ToString());
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
      new Nan::Callback(v8::Local<Function>::Cast(info[5]));
  LoadWorker *worker = new LoadWorker(baton, callback);

  if (!info[0]->IsUndefined() && !info[0]->IsNull())
    worker->SaveToPersistent("repo", info[0]->ToObject());
  if (!info[1]->IsUndefined() && !info[1]->IsNull())
    worker->SaveToPersistent("blob", info[1]->ToObject());
  if (!info[2]->IsUndefined() && !info[2]->IsNull())
    worker->SaveToPersistent("path", info[2]->ToObject());
  if (!info[3]->IsUndefined() && !info[3]->IsNull())
    worker->SaveToPersistent("mode", info[3]->ToObject());
  if (!info[4]->IsUndefined() && !info[4]->IsNull())
    worker->SaveToPersistent("flags", info[4]->ToObject());

  AsyncLibgit2QueueWorker(worker);
  return;
}

void GitFilterList::LoadWorker::Execute() {
  git_error_clear();

  {
    LockMaster lockMaster(
        /*asyncAction: */ true, baton->repo, baton->blob, baton->path);

    int result = git_filter_list_load(&baton->filters, baton->repo, baton->blob,
                                      baton->path, baton->mode, baton->flags);

    baton->error_code = result;

    if (result != GIT_OK && git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
}

void GitFilterList::LoadWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> to;
    // start convert_to_v8 block

    if (baton->filters != NULL) {
      // GitFilterList baton->filters
      v8::Local<v8::Array> owners = Nan::New<Array>(0);
      v8::Local<v8::Object> filterRegistry = Nan::New(GitFilterRegistry::persistentHandle);
      v8::Local<v8::Array> propertyNames = filterRegistry->GetPropertyNames();

      Nan::Set(
        owners,
        Nan::New<Number>(0),
        this->GetFromPersistent("repo")->ToObject()
      );

      for (uint32_t index = 0; index < propertyNames->Length(); ++index) {
        v8::Local<v8::String> propertyName = propertyNames->Get(index)->ToString();
        String::Utf8Value propertyNameAsUtf8Value(propertyName);
        const char *propertyNameAsCString = *propertyNameAsUtf8Value;

        bool isNotMethodOnRegistry = strcmp("register", propertyNameAsCString)
          && strcmp("unregister", propertyNameAsCString);
        if (isNotMethodOnRegistry && git_filter_list_contains(baton->filters, propertyNameAsCString)) {
          Nan::Set(
            owners,
            Nan::New<Number>(owners->Length()),
            filterRegistry->Get(propertyName)
          );
        }
      }

      to = GitFilterList::New(baton->filters, true, owners->ToObject());
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
        err = Nan::Error("Method load has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      err->Set(Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterList.load").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      std::queue<v8::Local<v8::Value>> workerArguments;
      workerArguments.push(GetFromPersistent("repo"));
      workerArguments.push(GetFromPersistent("blob"));
      workerArguments.push(GetFromPersistent("path"));
      workerArguments.push(GetFromPersistent("mode"));
      workerArguments.push(GetFromPersistent("flags"));
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
            Nan::Error("Method load has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(),
                 Nan::New(baton->error_code));
        err->Set(Nan::New("errorFunction").ToLocalChecked(),
                 Nan::New("FilterList.load").ToLocalChecked());
        v8::Local<v8::Value> argv[1] = {err};
        callback->Call(1, argv, async_resource);
      }
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton;
}
