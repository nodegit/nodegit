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
Napi::Value GitFilterList::Load(const Napi::CallbackInfo& info) {
  if (info.Length() == 0 || !info[0].IsObject()) {
    Napi::Error::New(info.Env(), "Repository repo is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (info.Length() == 2 || !info[2].IsString()) {
    Napi::Error::New(info.Env(), "String path is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (info.Length() == 3 || !info[3].IsNumber()) {
    Napi::Error::New(info.Env(), "Number mode is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (info.Length() == 4 || !info[4].IsNumber()) {
    Napi::Error::New(info.Env(), "Number flags is required.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  LoadBaton *baton = new LoadBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;

  // start convert_from_v8 block
  git_repository *from_repo = NULL;
  from_repo =
      NodeGitWrapper<GitRepositoryTraits>::Unwrap<GitRepository>(info[0].As<Napi::Object>())->GetValue();
  // end convert_from_v8 block
  baton->repo = from_repo;
  // start convert_from_v8 block
  git_blob *from_blob = NULL;
  if (info[1].IsObject()) {
    from_blob =
        NodeGitWrapper<GitBlobTraits>::Unwrap<GitBlob>(info[1].As<Napi::Object>())->GetValue();
  } else {
    from_blob = 0;
  }
  // end convert_from_v8 block
  baton->blob = from_blob;
  // start convert_from_v8 block
  const char *from_path = NULL;

  std::string pathStr = info[2].As<Napi::String>().Utf8Value();
  // malloc with one extra byte so we can add the terminating null character
  // C-strings expect:
  from_path = (const char *)malloc(pathStr.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead
  // of strdup or strcpy because nulls in the middle of strings are valid coming
  // from nodejs):
  memcpy((void *)from_path, pathStr.c_str(), pathStr.length());
  // ensure the final byte of our new string is null, extra casts added to
  // ensure compatibility with various C types used in the nodejs binding
  // generation:
  memset((void *)(((char *)from_path) + pathStr.length()), 0, 1);
  // end convert_from_v8 block
  baton->path = from_path;
  // start convert_from_v8 block
  git_filter_mode_t from_mode;
  from_mode = (git_filter_mode_t)(int)info[3].As<Napi::Number>().Int32Value();
  // end convert_from_v8 block
  baton->mode = from_mode;
  // start convert_from_v8 block
  uint32_t from_flags;
  from_flags = (uint32_t)info[4].As<Napi::Number>().Uint32Value();
  // end convert_from_v8 block
  baton->flags = from_flags;

  Napi::FunctionReference *callback =
      new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  LoadWorker *worker = new LoadWorker(baton, callback, cleanupHandles);

  worker->Reference<GitRepository>("repo", info[0]);
  worker->Reference<GitBlob>("blob", info[1]);

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
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

  if (result != GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
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
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->error_code == GIT_OK) {
    Napi::Value to;
    // start convert_to_v8 block

    if (baton->filters != NULL) {
      // GitFilterList baton->filters
      Napi::Array owners = Napi::Array::New(env, 0);
      nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();

      owners.Set(
        owners.Length(),
        this->GetFromPersistent("repo").As<Napi::Value>()
      );

      to = GitFilterList::New(baton->filters, true, owners.As<Napi::Object>());
      auto filterListWrapper = NodeGitWrapper<GitFilterListTraits>::Unwrap<GitFilterList>(to.As<Napi::Object>());
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
      to = env.Null();
    }

    // end convert_to_v8 block
    Napi::Value result = to;

    Napi::Value argv[2] = {env.Null(), result};
    CallCallback(argv, 2);
  } else {
    if (baton->error) {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method load has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "FilterList.load"));
      Napi::Value argv[1] = {err};
      CallCallback(argv, 1);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
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

      if (!callbackFired) {
        Napi::Object err =
            Napi::Error::New(env, "Method load has thrown an error.").Value();
        err.Set("errno", Napi::Number::New(env, baton->error_code));
        err.Set("errorFunction", Napi::String::New(env, "FilterList.load"));
        Napi::Value argv[1] = {err};
        CallCallback(argv, 1);
      }
    } else {
      CallCallback(nullptr, 0);
    }
  }

  free((void *)baton->path);

  delete baton;
}
