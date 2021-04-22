NAN_METHOD(GitRepository::GetRemotes)
{
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  GetRemotesBaton* baton = new GetRemotesBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = new std::vector<git_remote *>;
  baton->repo = Nan::ObjectWrap::Unwrap<GitRepository>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  GetRemotesWorker *worker = new GetRemotesWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info.This());
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitRepository::GetRemotesWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitRepository::GetRemotesWorker::Execute()
{
  giterr_clear();

  git_repository *repo;
  {
    nodegit::LockMaster lockMaster(true, baton->repo);
    baton->error_code = git_repository_open(&repo, git_repository_workdir(baton->repo));
  }

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete baton->out;
    baton->out = NULL;
    return;
  }

  git_strarray remote_names;
  baton->error_code = git_remote_list(&remote_names, repo);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete baton->out;
    baton->out = NULL;
    return;
  }

  for (size_t remote_index = 0; remote_index < remote_names.count; ++remote_index) {
    git_remote *remote;
    baton->error_code = git_remote_lookup(&remote, repo, remote_names.strings[remote_index]);

    // stop execution and return if there is an error
    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      // unwind and return
      while (baton->out->size()) {
        git_remote *remoteToFree = baton->out->back();
        baton->out->pop_back();
        git_remote_free(remoteToFree);
      }

      git_strarray_free(&remote_names);
      git_repository_free(repo);
      delete baton->out;
      baton->out = NULL;
      return;
    }

    baton->out->push_back(remote);
  }
}

void GitRepository::GetRemotesWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  while (baton->out->size()) {
    git_remote *remoteToFree = baton->out->back();
    baton->out->pop_back();
    git_remote_free(remoteToFree);
  }

  delete baton->out;

  delete baton;
}

void GitRepository::GetRemotesWorker::HandleOKCallback()
{
  if (baton->out != NULL)
  {
    unsigned int size = baton->out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      git_remote *remote = baton->out->at(i);
      Nan::Set(
        result,
        Nan::New<Number>(i),
        GitRemote::New(
          remote,
          true,
          Nan::To<v8::Object>(GitRepository::New(git_remote_owner(remote), true)).ToLocalChecked()
        )
      );
    }

    delete baton->out;

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else if (baton->error)
  {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
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
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Repository refreshRemotes has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Repository.refreshRemotes").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else
  {
    callback->Call(0, NULL, async_resource);
  }

  delete baton;
}
