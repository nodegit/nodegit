Napi::Value GitPatch::ConvenientFromDiff(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() == 0 || !info[0].IsObject()) {
    Napi::Error::New(env, "Diff diff is required.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  ConvenientFromDiffBaton *baton = new ConvenientFromDiffBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;

  baton->diff = NodeGitWrapper<GitDiffTraits>::Unwrap<GitDiff>(info[0].As<Napi::Object>())->GetValue();

  if (info[1].IsArray()) {
    const Napi::Array indexesArray = info[1].As<Napi::Array>();
    const uint32_t numIndexes = indexesArray.Length();

    for (uint32_t i = 0; i < numIndexes; ++i) {
      Napi::Value value = indexesArray.Get(i);
      int idx = value.As<Napi::Number>().Int32Value();
      baton->indexes.push_back(idx);
    }
  }

  baton->out = new std::vector<PatchData *>;
  baton->out->reserve(git_diff_num_deltas(baton->diff));

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  ConvenientFromDiffWorker *worker = new ConvenientFromDiffWorker(baton, callback, cleanupHandles);

  worker->Reference<GitDiff>("diff", info[0]);

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return env.Undefined();
}

nodegit::LockMaster GitPatch::ConvenientFromDiffWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->diff);
  return lockMaster;
}

void GitPatch::ConvenientFromDiffWorker::Execute() {
  git_error_clear();

  std::vector<git_patch *> patchesToBeFreed;

  if (baton->indexes.size() > 0) {
    for (int idx : baton->indexes) {
      git_patch *nextPatch;
      int result = git_patch_from_diff(&nextPatch, baton->diff, idx);

      if (result) {
        while (!patchesToBeFreed.empty())
        {
          git_patch_free(patchesToBeFreed.back());
          patchesToBeFreed.pop_back();
        }

        while (!baton->out->empty()) {
          PatchDataFree(baton->out->back());
          baton->out->pop_back();
        }

        baton->error_code = result;

        if (git_error_last()->klass != GIT_ERROR_NONE) {
          baton->error = git_error_dup(git_error_last());
        }

        delete baton->out;
        baton->out = NULL;

        return;
      }

      if (nextPatch != NULL) {
        baton->out->push_back(createFromRaw(nextPatch));
        patchesToBeFreed.push_back(nextPatch);
      }
    }
  } else {
    for (std::size_t i = 0; i < git_diff_num_deltas(baton->diff); ++i) {
      git_patch *nextPatch;
      int result = git_patch_from_diff(&nextPatch, baton->diff, i);

      if (result) {
        while (!patchesToBeFreed.empty())
        {
          git_patch_free(patchesToBeFreed.back());
          patchesToBeFreed.pop_back();
        }

        while (!baton->out->empty()) {
          PatchDataFree(baton->out->back());
          baton->out->pop_back();
        }

        baton->error_code = result;

        if (git_error_last()->klass != GIT_ERROR_NONE) {
          baton->error = git_error_dup(git_error_last());
        }

        delete baton->out;
        baton->out = NULL;

        return;
      }

      if (nextPatch != NULL) {
        baton->out->push_back(createFromRaw(nextPatch));
        patchesToBeFreed.push_back(nextPatch);
      }
    }
  }

  while (!patchesToBeFreed.empty())
  {
    git_patch_free(patchesToBeFreed.back());
    patchesToBeFreed.pop_back();
  }
}

void GitPatch::ConvenientFromDiffWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  while (!baton->out->empty()) {
    PatchDataFree(baton->out->back());
    baton->out->pop_back();
  }

  delete baton->out;

  delete baton;
}

void GitPatch::ConvenientFromDiffWorker::HandleOKCallback() {
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL) {
    unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);

    for (unsigned int i = 0; i < size; ++i) {
      result.Set(i, ConvenientPatch::New((void *)baton->out->at(i)));
    }

    delete baton->out;

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);

    return;
  }

  if (baton->error) {
    Napi::Object err;
    if (baton->error->message) {
      err = Napi::Error::New(env, baton->error->message).Value();
    } else {
      err = Napi::Error::New(env, "Method convenientFromDiff has thrown an error.").Value();
    }
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Patch.convenientFromDiff"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);

    return;
  }

  if (baton->error_code < 0) {
    Napi::Object err = Napi::Error::New(env, "method convenientFromDiff has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Patch.convenientFromDiff"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);

    return;
  }

  CallCallback(nullptr, 0);
}
