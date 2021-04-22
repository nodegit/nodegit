// NOTE you may need to occasionally rebuild this method by calling the generators
// if major changes are made to the templates / generator.

// Due to some garbage collection issues related to submodules and git_filters, we need to clone the repository
// pointer before giving it to a user.

/*
 * @param Repository callback
 */
NAN_METHOD(GitFilterSource::Repo) {
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  RepoBaton *baton = new RepoBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->src = Nan::ObjectWrap::Unwrap<GitFilterSource>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(v8::Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  RepoWorker *worker = new RepoWorker(baton, callback, cleanupHandles);

  worker->Reference<GitFilterSource>("src", info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitFilterSource::RepoWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->src);
  return lockMaster;
}

void GitFilterSource::RepoWorker::Execute() {
  git_error_clear();

  git_repository *repo = git_filter_source_repo(baton->src);
  baton->error_code = git_repository_open(&repo, git_repository_path(repo));

  if (baton->error_code == GIT_OK) {
    baton->out = repo;
  } else if (git_error_last() != NULL) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitFilterSource::RepoWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  git_repository_free(baton->out);

  delete baton;
}

void GitFilterSource::RepoWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> to;

    if (baton->out != NULL) {
      to = GitRepository::New(baton->out, true);
    } else {
      to = Nan::Null();
    }

    v8::Local<v8::Value> argv[2] = {Nan::Null(), to};
    callback->Call(2, argv, async_resource);
  } else {
    if (baton->error) {
      v8::Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
      } else {
        err = Nan::To<v8::Object>(Nan::Error("Method repo has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterSource.repo").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      v8::Local<v8::Object> err =
          Nan::To<v8::Object>(Nan::Error("Method repo has thrown an error.")).ToLocalChecked();
      Nan::Set(err, Nan::New("errno").ToLocalChecked(),
               Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterSource.repo").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton;
}
