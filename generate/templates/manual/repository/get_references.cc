Napi::Value GitRepository::GetReferences(const Napi::CallbackInfo& info)
{
  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  GetReferencesBaton* baton = new GetReferencesBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_reference *>;
  baton->repo = NodeGitWrapper<GitRepositoryTraits>::Unwrap<GitRepository>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  GetReferencesWorker *worker = new GetReferencesWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info.This().As<Napi::Object>());
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
}

nodegit::LockMaster GitRepository::GetReferencesWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->repo);
  return lockMaster;
}

void GitRepository::GetReferencesWorker::Execute()
{
  giterr_clear();

  git_repository *repo = baton->repo;

  git_strarray reference_names;
  baton->error_code = git_reference_list(&reference_names, repo);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete baton->out;
    baton->out = NULL;
    return;
  }

  for (size_t reference_index = 0; reference_index < reference_names.count; ++reference_index) {
    git_reference *reference;
    baton->error_code = git_reference_lookup(&reference, repo, reference_names.strings[reference_index]);

    // stop execution and return if there is an error
    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      // unwind and return
      while (baton->out->size()) {
        git_reference *referenceToFree = baton->out->back();
        baton->out->pop_back();
        git_reference_free(referenceToFree);
      }

      git_strarray_free(&reference_names);
      delete baton->out;
      baton->out = NULL;
      return;
    }

    if (git_reference_type(reference) == GIT_REF_SYMBOLIC) {
      git_reference *resolved_reference;
      int resolve_result = git_reference_resolve(&resolved_reference, reference);
      git_reference_free(reference);

      // if we can't resolve the ref, then just ignore it
      if (resolve_result == GIT_OK) {
        baton->out->push_back(resolved_reference);
      }
    } else {
      baton->out->push_back(reference);
    }
  }

  git_strarray_free(&reference_names);
}

void GitRepository::GetReferencesWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  while (baton->out->size()) {
    git_reference *referenceToFree = baton->out->back();
    baton->out->pop_back();
    git_reference_free(referenceToFree);
  }

  delete baton->out;

  delete baton;
}

void GitRepository::GetReferencesWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      git_reference *reference = baton->out->at(i);
      result.Set(
        i,
        GitRefs::New(
          reference,
          true,
          GitRepository::New(git_reference_owner(reference), true).As<Napi::Object>()
        )
      );
    }

    delete baton->out;

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
    Napi::Object err = Napi::Error::New(env, "Repository getReferences has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Repository.getReferences"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  }
  else
  {
    CallCallback(nullptr, 0);
  }

  delete baton;
}
