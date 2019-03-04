// NOTE you may need to occasionally rebuild this method by calling the generators
// if major changes are made to the templates / generator.

// Due to some garbage collection issues related to submodules and git_filters, we need to clone the repository
// pointer before giving it to a user.

/*
 * @param Repository callback
 */
NAN_METHOD(GitFilterSource::Repo) {
  if (info.Length() == 0 || !info[0]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  RepoBaton *baton = new RepoBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->src = Nan::ObjectWrap::Unwrap<GitFilterSource>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(v8::Local<Function>::Cast(info[0]));
  RepoWorker *worker = new RepoWorker(baton, callback);

  worker->SaveToPersistent("src", info.This());

  AsyncLibgit2QueueWorker(worker);
  return;
}

void GitFilterSource::RepoWorker::Execute() {
  git_error_clear();

  {
    LockMaster lockMaster(true, baton->src);

    git_repository *repo = git_filter_source_repo(baton->src);
    baton->error_code = git_repository_open(&repo, git_repository_path(repo));

    if (baton->error_code == GIT_OK) {
      baton->out = repo;
    } else if (git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
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
        err = Nan::Error(baton->error->message)->ToObject();
      } else {
        err = Nan::Error("Method repo has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      err->Set(Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterSource.repo").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      v8::Local<v8::Object> err =
          Nan::Error("Method repo has thrown an error.")->ToObject();
      err->Set(Nan::New("errno").ToLocalChecked(),
               Nan::New(baton->error_code));
      err->Set(Nan::New("errorFunction").ToLocalChecked(),
               Nan::New("FilterSource.repo").ToLocalChecked());
      v8::Local<v8::Value> argv[1] = {err};
      callback->Call(1, argv, async_resource);
    } else {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton;
}
