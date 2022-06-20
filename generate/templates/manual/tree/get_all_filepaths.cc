
namespace TreeFilepathsHelpers {

int iterateTreePaths(git_repository *repo, git_tree *tree, std::vector<std::string> *paths,std::string *buffer) {
  size_t size = git_tree_entrycount(tree);
  for (size_t i = 0; i < size; i++) {
    const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
    const git_filemode_t filemode = git_tree_entry_filemode(entry);
    if (filemode == GIT_FILEMODE_BLOB || filemode == GIT_FILEMODE_BLOB_EXECUTABLE) {
      paths->push_back(*buffer + std::string(git_tree_entry_name(entry)));
    }
    else if (filemode == GIT_FILEMODE_TREE) {
      git_tree *subtree;
      int error = git_tree_lookup(&subtree, repo, git_tree_entry_id(entry));
      if (error == GIT_OK) {
        size_t size = buffer->size();
        /* append the next entry to the path */
        buffer->append(git_tree_entry_name(entry));
        buffer->append("/");
        error = iterateTreePaths(repo, subtree, paths, buffer);
        git_tree_free(subtree);
        buffer->resize(size);
      }

      if (error < 0 ) {
        return error;
      }

    }
  }
  return GIT_OK;
}

} // end anonymous namespace

NAN_METHOD(GitTree::GetAllFilepaths)
{
  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  GetAllFilepathsBaton* baton = new GetAllFilepathsBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->tree = Nan::ObjectWrap::Unwrap<GitTree>(info.This())->GetValue();
  baton->out = new std::vector<std::string>;
  baton->repo = git_tree_owner(baton->tree);

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  GetAllFilepathsWorker *worker = new GetAllFilepathsWorker(baton, callback, cleanupHandles);
  worker->Reference<GitTree>("tree", info.This());
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);

  return;
}

nodegit::LockMaster GitTree::GetAllFilepathsWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->tree, baton->repo);
  return lockMaster;
}

void GitTree::GetAllFilepathsWorker::Execute()
{
  std::string buffer;
  buffer.reserve(4096);
  baton->error_code = TreeFilepathsHelpers::iterateTreePaths(baton->repo, baton->tree, baton->out, &buffer);
  if (baton->error_code != GIT_OK && git_error_last() != NULL) {
    baton->error = git_error_dup(git_error_last());
  }
}

void GitTree::GetAllFilepathsWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  delete baton->out;

  delete baton;
}

void GitTree::GetAllFilepathsWorker::HandleOKCallback()
{
  if (baton->error_code == GIT_OK) {
    std::vector<std::string> &paths = *(baton->out);
    v8::Local<v8::Array> result = Nan::New<v8::Array>(paths.size());
    for (unsigned int i = 0; i < paths.size(); i++) {
      Nan::Set(result, i, Nan::New<v8::String>(paths[i]).ToLocalChecked());
    }

    v8::Local<v8::Value> argv[2] = {Nan::Null(), result};
    callback->Call(2, argv, async_resource);
  }
  else
  {
    if (baton->error)
    {
      Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
      } else {
        err = Nan::To<v8::Object>(Nan::Error("Method getAllFilepaths has thrown an error.")).ToLocalChecked();
      }
      Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Tree.getAllFilepaths").ToLocalChecked());
      Local<v8::Value> argv[1] = {
        err
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

      if (!callbackFired)
      {
        Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method getAllFilepaths has thrown an error.")).ToLocalChecked();
        Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.getAllFilepaths").ToLocalChecked());
        Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv, async_resource);
      }
    }
    else
    {
      callback->Call(0, NULL, async_resource);
    }
  }

  delete baton->out;
  delete baton;
}
