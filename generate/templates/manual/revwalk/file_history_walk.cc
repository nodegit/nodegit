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
  }

  v8::Local<v8::Value> toJavascript() {
    v8::Local<v8::Object> historyEntry = Nan::New<v8::Object>();
    v8::Local<v8::Array> owners = Nan::New<Array>(0);
    Nan::Set(
      owners,
      Nan::New<v8::Number>(owners->Length()),
      Nan::To<v8::Object>(GitRepository::New(
        git_commit_owner(commit),
        true
      )).ToLocalChecked()
    );
    Nan::Set(historyEntry, Nan::New("commit").ToLocalChecked(), GitCommit::New(commit, true, owners));
    commit = NULL;
    Nan::Set(historyEntry, Nan::New("status").ToLocalChecked(), Nan::New<Number>(type));
    Nan::Set(historyEntry, Nan::New("isMergeCommit").ToLocalChecked(), Nan::New(isMergeCommit));
    if (type == GIT_DELTA_RENAMED) {
      if (from != NULL) {
        Nan::Set(historyEntry, Nan::New("oldName").ToLocalChecked(), Nan::New(from).ToLocalChecked());
      }
      if (to != NULL) {
        Nan::Set(historyEntry, Nan::New("newName").ToLocalChecked(), Nan::New(to).ToLocalChecked());
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

NAN_METHOD(GitRevwalk::FileHistoryWalk)
{
  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("File path to get the history is required.");
  }

  if (info.Length() == 1 || !info[1]->IsNumber()) {
    return Nan::ThrowError("Max count is required and must be a number.");
  }

  if (!info[info.Length() - 1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FileHistoryWalkBaton* baton = new FileHistoryWalkBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  Nan::Utf8String from_js_file_path(Nan::To<v8::String>(info[0]).ToLocalChecked());
  baton->file_path = strdup(*from_js_file_path);
  baton->max_count = Nan::To<unsigned int>(info[1]).FromJust();
  baton->out = new std::vector<void *>;
  baton->out->reserve(baton->max_count);
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[info.Length() - 1]));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  FileHistoryWalkWorker *worker = new FileHistoryWalkWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRevwalk>("fileHistoryWalk", info.This());

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
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
  if (baton->out != NULL) {
    const unsigned int size = baton->out->size();
    v8::Local<v8::Array> result = Nan::New<v8::Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      FileHistoryEvent *batonResult = static_cast<FileHistoryEvent *>(baton->out->at(i));
      Nan::Set(result, Nan::New(i), batonResult->toJavascript());
      delete batonResult;
    }

    Nan::Set(result, Nan::New("reachedEndOfHistory").ToLocalChecked(), Nan::New(baton->error_code == GIT_ITEROVER));

    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);

    delete baton->out;
    return;
  }

  if (baton->error) {
    v8::Local<v8::Object> err;
    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method fileHistoryWalk has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.fileHistoryWalk").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
    return;
  }

  if (baton->error_code < 0) {
    v8::Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method next has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Revwalk.fileHistoryWalk").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
    return;
  }

  callback->Call(0, NULL, async_resource);

  delete baton;
}
