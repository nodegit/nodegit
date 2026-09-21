
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

Napi::Value GitTree::GetAllFilepaths(const Napi::CallbackInfo& info)
{
  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  GetAllFilepathsBaton* baton = new GetAllFilepathsBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->tree = NodeGitWrapper<GitTreeTraits>::Unwrap<GitTree>(info.This().As<Napi::Object>())->GetValue();
  baton->out = new std::vector<std::string>;
  baton->repo = git_tree_owner(baton->tree);

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  GetAllFilepathsWorker *worker = new GetAllFilepathsWorker(baton, callback, cleanupHandles);
  worker->Reference<GitTree>("tree", info.This().As<Napi::Object>());
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);

  return info.Env().Undefined();
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
  if (baton->error_code != GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
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
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->error_code == GIT_OK) {
    std::vector<std::string> &paths = *(baton->out);
    Napi::Array result = Napi::Array::New(env, paths.size());
    for (unsigned int i = 0; i < paths.size(); i++) {
      result.Set(i, Napi::String::New(env, paths[i]));
    }

    Napi::Value argv[2] = {env.Null(), result};
    CallCallback(argv, 2);
  }
  else
  {
    if (baton->error)
    {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method getAllFilepaths has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "Tree.getAllFilepaths"));
      Napi::Value argv[1] = {
        err
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

      if (!callbackFired)
      {
        Napi::Object err = Napi::Error::New(env, "Method getAllFilepaths has thrown an error.").Value();
        err.Set("errno", Napi::Number::New(env, baton->error_code));
        err.Set("errorFunction", Napi::String::New(env, "Revwalk.getAllFilepaths"));
        Napi::Value argv[1] = {
          err
        };
        CallCallback(argv, 1);
      }
    }
    else
    {
      CallCallback(nullptr, 0);
    }
  }

  delete baton->out;
  delete baton;
}
