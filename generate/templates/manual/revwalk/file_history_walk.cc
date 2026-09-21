// Note: commit is not owned by this class (must be freed elsewhere)
class FileHistoryEvent {
public:
  FileHistoryEvent(
    git_delta_t inputType,
    bool inputExistsInCurrentTree,
    bool inputIsMergeCommit,
    git_commit *inputCommit,
    const char *inputFrom,
    const char *inputTo
  ):
    type(inputType),
    existsInCurrentTree(inputExistsInCurrentTree),
    isMergeCommit(inputIsMergeCommit),
    from(inputFrom == NULL ? NULL : strdup(inputFrom)),
    to(inputTo == NULL ? NULL : strdup(inputTo)),
    commit(inputCommit)
  {
    if (inputCommit != NULL) {
      const int error = git_commit_dup(&commit, inputCommit);
      assert(error == GIT_OK);
    }
  }

  FileHistoryEvent(const FileHistoryEvent &) = delete;
  FileHistoryEvent(FileHistoryEvent &&) = delete;
  FileHistoryEvent &operator=(const FileHistoryEvent &) = delete;
  FileHistoryEvent &operator=(FileHistoryEvent &&) = delete;

  ~FileHistoryEvent() {
    if (commit != NULL) {
      git_commit_free(commit);
    }

    if(from != NULL) {
      free((void *)from);
    }

    if(to != NULL) {
      free((void *)to);
    }
  }

  Napi::Value toJavascript(Napi::Env env) {
    Napi::Object historyEntry = Napi::Object::New(env);
    Napi::Array owners = Napi::Array::New(env, 0);
    owners.Set(
      owners.Length(),
      GitRepository::New(
        git_commit_owner(commit),
        true
      ).As<Napi::Object>()
    );
    historyEntry.Set("commit", GitCommit::New(commit, true, owners));
    commit = NULL;
    historyEntry.Set("status", Napi::Number::New(env, type));
    historyEntry.Set("isMergeCommit", Napi::Boolean::New(env, isMergeCommit));
    if (type == GIT_DELTA_RENAMED) {
      if (from != NULL) {
        historyEntry.Set("oldName", Napi::String::New(env, from));
      }
      if (to != NULL) {
        historyEntry.Set("newName", Napi::String::New(env, to));
      }
    }
    return historyEntry;
  }

  static int buildHistoryEvent(
    FileHistoryEvent **fileHistoryEvent,
    git_repository *repo,
    git_commit *currentCommit,
    git_tree *currentTree,
    git_tree *parentTree,
    const char *filePath
  ) {
    int errorCode;
    git_tree_entry *currentEntry;
    if (git_tree_entry_bypath(&currentEntry, currentTree, filePath) != GIT_OK) {
      currentEntry = NULL;
    }
    git_tree_entry *parentEntry;
    if (git_tree_entry_bypath(&parentEntry, parentTree, filePath) != GIT_OK) {
      parentEntry = NULL;
    }

    if (!currentEntry && !parentEntry) {
      *fileHistoryEvent = new FileHistoryEvent(GIT_DELTA_UNMODIFIED, false, false, currentCommit, NULL, NULL);
      return GIT_OK;
    }

    // The filePath was added
    if (currentEntry && !parentEntry) {
      git_diff *diff;
      if ((errorCode = git_diff_tree_to_tree(&diff, repo, parentTree, currentTree, NULL)) != GIT_OK) {
        git_tree_entry_free(currentEntry);
        return errorCode;
      }
      if ((errorCode = git_diff_find_similar(diff, NULL)) != GIT_OK) {
        git_diff_free(diff);
        git_tree_entry_free(currentEntry);
        return errorCode;
      }
      const size_t numDeltas = git_diff_num_deltas(diff);
      for (size_t i = 0; i < numDeltas; ++i) {
        const git_diff_delta *delta = git_diff_get_delta(diff, i);
        if (delta->new_file.path != NULL && std::strcmp(delta->new_file.path, filePath) == 0) {
          if (delta->status == GIT_DELTA_RENAMED
            || (delta->old_file.path != NULL && std::strcmp(delta->old_file.path, filePath) != 0)) {
            *fileHistoryEvent = new FileHistoryEvent(
              GIT_DELTA_RENAMED,
              true,
              false,
              currentCommit,
              delta->old_file.path,
              delta->new_file.path
            );
            git_diff_free(diff);
            git_tree_entry_free(currentEntry);
            return GIT_OK;
          }
          break;
        }
      }
      git_diff_free(diff);
      git_tree_entry_free(currentEntry);

      *fileHistoryEvent = new FileHistoryEvent(GIT_DELTA_ADDED, true, false, currentCommit, NULL, NULL);
      return GIT_OK;
    }

    // The filePath was deleted
    if (!currentEntry && parentEntry) {
      git_diff *diff;
      if ((errorCode = git_diff_tree_to_tree(&diff, repo, parentTree, currentTree, NULL)) != GIT_OK) {
        git_tree_entry_free(parentEntry);
        return errorCode;
      }
      if ((errorCode = git_diff_find_similar(diff, NULL)) != GIT_OK) {
        git_diff_free(diff);
        git_tree_entry_free(parentEntry);
        return errorCode;
      }
      const size_t numDeltas = git_diff_num_deltas(diff);
      for (size_t i = 0; i < numDeltas; ++i) {
        const git_diff_delta *delta = git_diff_get_delta(diff, i);
        if (delta->old_file.path != NULL && std::strcmp(delta->old_file.path, filePath) == 0) {
          if (delta->status == GIT_DELTA_RENAMED
            || (delta->new_file.path != NULL && std::strcmp(delta->new_file.path, filePath) != 0)) {
            *fileHistoryEvent = new FileHistoryEvent(
              GIT_DELTA_RENAMED,
              false,
              false,
              currentCommit,
              delta->old_file.path,
              delta->new_file.path
            );
            git_diff_free(diff);
            git_tree_entry_free(parentEntry);
            return GIT_OK;
          }
          break;
        }
      }
      git_diff_free(diff);
      git_tree_entry_free(parentEntry);

      *fileHistoryEvent =  new FileHistoryEvent(GIT_DELTA_DELETED, false, false, currentCommit, NULL, NULL);
      return GIT_OK;
    }

    if (git_oid_cmp(git_tree_entry_id(currentEntry), git_tree_entry_id(parentEntry)) != 0
        || git_tree_entry_filemode(currentEntry) != git_tree_entry_filemode(parentEntry)
    ) {
      git_tree_entry_free(parentEntry);
      git_tree_entry_free(currentEntry);
      *fileHistoryEvent = new FileHistoryEvent(GIT_DELTA_MODIFIED, true, false, currentCommit, NULL, NULL);
      return GIT_OK;
    }

    *fileHistoryEvent = new FileHistoryEvent(GIT_DELTA_UNMODIFIED, true, false, currentCommit, NULL, NULL);
    git_tree_entry_free(parentEntry);
    git_tree_entry_free(currentEntry);
    return GIT_OK;
  }

  git_delta_t type;
  bool existsInCurrentTree, isMergeCommit;
  const char *from, *to;
  git_commit *commit;
};

Napi::Value GitRevwalk::FileHistoryWalk(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();
  if (info.Length() == 0 || !info[0].IsString()) {
    Napi::Error::New(env, "File path to get the history is required.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (info.Length() == 1 || !info[1].IsNumber()) {
    Napi::Error::New(env, "Max count is required and must be a number.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  FileHistoryWalkBaton* baton = new FileHistoryWalkBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  std::string from_js_file_path = info[0].As<Napi::String>().Utf8Value();
  baton->file_path = strdup(from_js_file_path.c_str());
  baton->max_count = info[1].As<Napi::Number>().Uint32Value();
  baton->out = new std::vector<void *>;
  baton->out->reserve(baton->max_count);
  baton->walk = NodeGitWrapper<GitRevwalkTraits>::Unwrap<GitRevwalk>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  FileHistoryWalkWorker *worker = new FileHistoryWalkWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("fileHistoryWalk", info.This().As<Napi::Object>());

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return env.Undefined();
}

nodegit::LockMaster GitRevwalk::FileHistoryWalkWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true);
  return lockMaster;
}

void GitRevwalk::FileHistoryWalkWorker::Execute()
{
  git_repository *repo = git_revwalk_repository(baton->walk);
  git_oid currentOid;
  git_error_clear();
  for (
    unsigned int revwalkIterations = 0;
    revwalkIterations < baton->max_count && (baton->error_code = git_revwalk_next(&currentOid, baton->walk)) == GIT_OK;
    ++revwalkIterations
  ) {
    git_commit *currentCommit;
    if ((baton->error_code = git_commit_lookup(&currentCommit, repo, &currentOid)) != GIT_OK) {
      break;
    }

    git_tree *currentTree;
    if ((baton->error_code = git_commit_tree(&currentTree, currentCommit)) != GIT_OK) {
      git_commit_free(currentCommit);
      break;
    }

    const unsigned int parentCount = git_commit_parentcount(currentCommit);
    if (parentCount == 0) {
      git_tree_entry* entry;
      if (git_tree_entry_bypath(&entry, currentTree, baton->file_path) == GIT_OK) {
        baton->out->push_back(new FileHistoryEvent(GIT_DELTA_ADDED, false, false, currentCommit, NULL, NULL));
        git_tree_entry_free(entry);
      }
      git_commit_free(currentCommit);
      git_tree_free(currentTree);
      continue;
    }

    if (parentCount == 1) {
      git_commit *parentCommit;
      if ((baton->error_code = git_commit_parent(&parentCommit, currentCommit, 0)) != GIT_OK) {
        git_commit_free(currentCommit);
        git_tree_free(currentTree);
        break;
      }

      git_tree *parentTree;
      if ((baton->error_code = git_commit_tree(&parentTree, parentCommit)) != GIT_OK) {
        git_commit_free(currentCommit);
        git_commit_free(parentCommit);
        git_tree_free(currentTree);
        break;
      }


      FileHistoryEvent *fileHistoryEvent;
      if ((baton->error_code = FileHistoryEvent::buildHistoryEvent(
        &fileHistoryEvent,
        repo,
        currentCommit,
        currentTree,
        parentTree,
        baton->file_path
      )) != GIT_OK) {
        git_commit_free(currentCommit);
        git_commit_free(parentCommit);
        git_tree_free(currentTree);
        git_tree_free(parentTree);
        break;
      }

      if (fileHistoryEvent->type != GIT_DELTA_UNMODIFIED) {
        baton->out->push_back(fileHistoryEvent);
      }

      git_commit_free(currentCommit);
      git_commit_free(parentCommit);
      git_tree_free(currentTree);
      git_tree_free(parentTree);
      continue;
    }

    std::pair<bool, unsigned int> firstMatchingParentIndex(false, 0);
    bool fileExistsInCurrent = false, fileExistsInSomeParent = false;
    for (unsigned int parentIndex = 0; parentIndex < parentCount; ++parentIndex) {
      git_commit *parentCommit;
      if ((baton->error_code = git_commit_parent(&parentCommit, currentCommit, parentIndex)) != GIT_OK) {
        break;
      }

      git_tree *parentTree;
      if ((baton->error_code = git_commit_tree(&parentTree, parentCommit)) != GIT_OK) {
        git_commit_free(parentCommit);
        break;
      }

      FileHistoryEvent *fileHistoryEvent;
      if ((baton->error_code = FileHistoryEvent::buildHistoryEvent(
        &fileHistoryEvent,
        repo,
        currentCommit,
        currentTree,
        parentTree,
        baton->file_path
      )) != GIT_OK) {
        git_tree_free(parentTree);
        git_commit_free(parentCommit);
        break;
      }

      switch (fileHistoryEvent->type) {
        case GIT_DELTA_ADDED: {
          fileExistsInCurrent = true;
          break;
        }
        case GIT_DELTA_MODIFIED: {
          fileExistsInCurrent = true;
          fileExistsInSomeParent = true;
          break;
        }
        case GIT_DELTA_DELETED: {
          fileExistsInSomeParent = true;
          break;
        }
        case GIT_DELTA_RENAMED: {
          if (fileHistoryEvent->existsInCurrentTree) {
            fileExistsInCurrent = true;
          } else {
            fileExistsInSomeParent = true;
          }
          break;
        }
        case GIT_DELTA_UNMODIFIED: {
          if (fileHistoryEvent->existsInCurrentTree) {
            fileExistsInCurrent = true;
            fileExistsInSomeParent = true;
          }
          firstMatchingParentIndex = std::make_pair(true, parentIndex);
          break;
        }
        default: {
          break;
        }
      }

      delete fileHistoryEvent;
      git_commit_free(parentCommit);
      git_tree_free(parentTree);

     if (firstMatchingParentIndex.first) {
        break;
      }
    }

    if (baton->error_code != GIT_OK) {
      git_tree_free(currentTree);
      git_commit_free(currentCommit);
      break;
    }

    if (!firstMatchingParentIndex.first) {
      assert(fileExistsInCurrent || fileExistsInSomeParent);
      git_delta_t mergeType = GIT_DELTA_UNREADABLE; // It will never result in this case because of the assertion above.
      if (fileExistsInCurrent && fileExistsInSomeParent) {
        mergeType = GIT_DELTA_MODIFIED;
      } else if (fileExistsInCurrent) {
        mergeType = GIT_DELTA_ADDED;
      } else if (fileExistsInSomeParent) {
        mergeType = GIT_DELTA_DELETED;
      }

      FileHistoryEvent *fileHistoryEvent = new FileHistoryEvent(
        mergeType,
        mergeType != GIT_DELTA_DELETED,
        true,
        currentCommit,
        NULL,
        NULL
      );
      baton->out->push_back(fileHistoryEvent);
      git_tree_free(currentTree);
      git_commit_free(currentCommit);
      continue;
    }

    assert(firstMatchingParentIndex.first);
    for (unsigned int parentIndex = 0; parentIndex < parentCount; ++parentIndex) {
      if (parentIndex == firstMatchingParentIndex.second) {
        continue;
      }

      const git_oid *parentOid = git_commit_parent_id(currentCommit, parentIndex);
      assert(parentOid != NULL);
      git_revwalk_hide(baton->walk, parentOid);
    }
    git_commit_free(currentCommit);
    git_tree_free(currentTree);
  }

  if (baton->error_code != GIT_OK && baton->error_code != GIT_ITEROVER) {
    // Something went wrong in our loop, discard everything in the async worker
    for (unsigned int i = 0; i < baton->out->size(); ++i) {
      delete static_cast<FileHistoryEvent *>(baton->out->at(i));
    }
    delete baton->out;
    baton->out = NULL;
    baton->error = git_error_dup(git_error_last());
  }
  free((void *)baton->file_path);
  baton->file_path = NULL;
}

void GitRevwalk::FileHistoryWalkWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  for (unsigned int i = 0; i < baton->out->size(); ++i) {
    delete static_cast<FileHistoryEvent *>(baton->out->at(i));
  }

  delete baton->out;

  free((void *)baton->file_path);

  delete baton;
}

void GitRevwalk::FileHistoryWalkWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL) {
    const unsigned int size = baton->out->size();
    Napi::Array result = Napi::Array::New(env, size);
    for (unsigned int i = 0; i < size; i++) {
      FileHistoryEvent *batonResult = static_cast<FileHistoryEvent *>(baton->out->at(i));
      result.Set(i, batonResult->toJavascript(env));
      delete batonResult;
    }

    result.Set("reachedEndOfHistory", Napi::Boolean::New(env, baton->error_code == GIT_ITEROVER));

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);

    delete baton->out;
    return;
  }

  if (baton->error) {
    Napi::Object err;
    if (baton->error->message) {
      err = Napi::Error::New(env, baton->error->message).Value();
    } else {
      err = Napi::Error::New(env, "Method fileHistoryWalk has thrown an error.").Value();
    }
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Revwalk.fileHistoryWalk"));
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
    Napi::Object err = Napi::Error::New(env, "Method next has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Revwalk.fileHistoryWalk"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
    return;
  }

  CallCallback(nullptr, 0);

  delete baton;
}
