NAN_METHOD(GitRevwalk::CommitWalk) {
  if (info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Max count is required and must be a number.");
  }

  if (info.Length() == 1 || !info[1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  CommitWalkBaton* baton = new CommitWalkBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->max_count = Nan::To<unsigned int>(info[0]).FromJust();
  baton->out = new std::vector<git_commit *>;
  baton->out->reserve(baton->max_count);
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));
  CommitWalkWorker *worker = new CommitWalkWorker(baton, callback);
  worker->SaveToPersistent("fastWalk", info.This());

  Nan::AsyncQueueWorker(worker);
  return;
}

void GitRevwalk::CommitWalkWorker::Execute() {
  giterr_clear();

  for (int i = 0; i < baton->max_count; i++) {
    git_oid next_commit_id;
    baton->error_code = git_revwalk_next(&next_commit_id, baton->walk);

    if (baton->error_code == GIT_ITEROVER) {
      baton->error_code = GIT_OK;
      return;
    }

    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      while (baton->out->size()) {
        git_commit_free(baton->out->back());
        baton->out->pop_back();
      }

      delete baton->out;
      baton->out = NULL;

      return;
    }

    git_commit *commit;
    baton->error_code = git_commit_lookup(&commit, git_revwalk_repository(baton->walk), &next_commit_id);

    if (baton->error_code != GIT_OK) {
      if (giterr_last() != NULL) {
        baton->error = git_error_dup(giterr_last());
      }

      while (baton->out->size()) {
        git_commit_free(baton->out->back());
        baton->out->pop_back();
      }

      delete baton->out;
      baton->out = NULL;

      return;
    }

    baton->out->push_back(commit);
  }
}

void GitRevwalk::CommitWalkWorker::HandleOKCallback() {
  if (baton->out != NULL) {
    unsigned int size = baton->out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      git_commit *commit = baton->out->at(i);
      Nan::Set(
        result,
        Nan::New<Number>(i),
        GitCommit::New(
          commit,
          true,
          GitRepository::New(git_commit_owner(commit), true)->ToObject()
        )
      );
    }

    delete baton->out;

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  } else if (baton->error) {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  } else if (baton->error_code < 0) {
    Local<v8::Object> err = Nan::Error("Revwalk commitWalk has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.commitWalk").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  } else {
    callback->Call(0, NULL, async_resource);
  }
}
