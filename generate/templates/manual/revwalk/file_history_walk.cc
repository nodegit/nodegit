NAN_METHOD(GitRevwalk::FileHistoryWalk)
{
  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("File path to get the history is required.");
  }

  if (info.Length() == 1 || !info[1]->IsNumber()) {
    return Nan::ThrowError("Max count is required and must be a number.");
  }

  if (info.Length() == 2 || !info[2]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FileHistoryWalkBaton* baton = new FileHistoryWalkBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  String::Utf8Value from_js_file_path(info[0]->ToString());
  baton->file_path = strdup(*from_js_file_path);
  baton->max_count = (unsigned int)info[1]->ToNumber()->Value();
  baton->out = new std::vector< std::pair<git_commit *, std::pair<char *, git_delta_t> > *>;
  baton->out->reserve(baton->max_count);
  baton->walk = Nan::ObjectWrap::Unwrap<GitRevwalk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[2]));
  FileHistoryWalkWorker *worker = new FileHistoryWalkWorker(baton, callback);
  worker->SaveToPersistent("fileHistoryWalk", info.This());

  Nan::AsyncQueueWorker(worker);
  return;
}

void GitRevwalk::FileHistoryWalkWorker::Execute()
{
  git_repository *repo = git_revwalk_repository(baton->walk);
  git_oid *nextOid = (git_oid *)malloc(sizeof(git_oid));
  giterr_clear();
  for (
    unsigned int i = 0;
    i < baton->max_count && (baton->error_code = git_revwalk_next(nextOid, baton->walk)) == GIT_OK;
    ++i
  ) {
    // check if this commit has the file
    git_commit *nextCommit;

    if ((baton->error_code = git_commit_lookup(&nextCommit, repo, nextOid)) != GIT_OK) {
      break;
    }

    git_tree *thisTree, *parentTree;
    if ((baton->error_code = git_commit_tree(&thisTree, nextCommit)) != GIT_OK) {
      git_commit_free(nextCommit);
      break;
    }

    git_diff *diffs;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    char *file_path = strdup(baton->file_path);
    opts.pathspec.strings = &file_path;
    opts.pathspec.count = 1;
    git_commit *parent;
    unsigned int parents = git_commit_parentcount(nextCommit);
    if (parents > 1) {
      git_commit_free(nextCommit);
      continue;
    } else if (parents == 1) {
      if ((baton->error_code = git_commit_parent(&parent, nextCommit, 0)) != GIT_OK) {
        git_commit_free(nextCommit);
        break;
      }
      if (
        (baton->error_code = git_commit_tree(&parentTree, parent)) != GIT_OK ||
        (baton->error_code = git_diff_tree_to_tree(&diffs, repo, parentTree, thisTree, &opts)) != GIT_OK
      ) {
        git_commit_free(nextCommit);
        git_commit_free(parent);
        break;
      }
    } else {
      if ((baton->error_code = git_diff_tree_to_tree(&diffs, repo, NULL, thisTree, &opts)) != GIT_OK) {
        git_commit_free(nextCommit);
        break;
      }
    }

    free(file_path);
    opts.pathspec.strings = NULL;
    opts.pathspec.count = 0;

    bool flag = false;
    bool doRenamedPass = false;
    unsigned int numDeltas = git_diff_num_deltas(diffs);
    for (unsigned int j = 0; j < numDeltas; ++j) {
      git_patch *nextPatch;
      baton->error_code = git_patch_from_diff(&nextPatch, diffs, j);

      if (baton->error_code < GIT_OK) {
        break;
      }

      if (nextPatch == NULL) {
        continue;
      }

      const git_diff_delta *delta = git_patch_get_delta(nextPatch);
      bool isEqualOldFile = !strcmp(delta->old_file.path, baton->file_path);
      bool isEqualNewFile = !strcmp(delta->new_file.path, baton->file_path);

      if (isEqualNewFile) {
        if (delta->status == GIT_DELTA_ADDED || delta->status == GIT_DELTA_DELETED) {
          doRenamedPass = true;
          break;
        }
        std::pair<git_commit *, std::pair<char *, git_delta_t> > *historyEntry;
        if (!isEqualOldFile) {
          historyEntry = new std::pair<git_commit *, std::pair<char *, git_delta_t> >(
            nextCommit,
            std::pair<char *, git_delta_t>(strdup(delta->old_file.path), delta->status)
          );
        } else {
          historyEntry = new std::pair<git_commit *, std::pair<char *, git_delta_t> >(
            nextCommit,
            std::pair<char *, git_delta_t>(strdup(delta->new_file.path), delta->status)
          );
        }
        baton->out->push_back(historyEntry);
        flag = true;
      }

      git_patch_free(nextPatch);

      if (flag) {
        break;
      }
    }

    if (doRenamedPass) {
      git_diff_free(diffs);

      if (parents == 1) {
        if ((baton->error_code = git_diff_tree_to_tree(&diffs, repo, parentTree, thisTree, NULL)) != GIT_OK) {
          git_commit_free(nextCommit);
          break;
        }
        if ((baton->error_code = git_diff_find_similar(diffs, NULL)) != GIT_OK) {
          git_commit_free(nextCommit);
          break;
        }
      } else {
        if ((baton->error_code = git_diff_tree_to_tree(&diffs, repo, NULL, thisTree, NULL)) != GIT_OK) {
          git_commit_free(nextCommit);
          break;
        }
        if((baton->error_code = git_diff_find_similar(diffs, NULL)) != GIT_OK) {
          git_commit_free(nextCommit);
          break;
        }
      }

      flag = false;
      numDeltas = git_diff_num_deltas(diffs);
      for (unsigned int j = 0; j < numDeltas; ++j) {
        git_patch *nextPatch;
        baton->error_code = git_patch_from_diff(&nextPatch, diffs, j);

        if (baton->error_code < GIT_OK) {
          break;
        }

        if (nextPatch == NULL) {
          continue;
        }

        const git_diff_delta *delta = git_patch_get_delta(nextPatch);
        bool isEqualOldFile = !strcmp(delta->old_file.path, baton->file_path);
        bool isEqualNewFile = !strcmp(delta->new_file.path, baton->file_path);
        int oldLen = strlen(delta->old_file.path);
        int newLen = strlen(delta->new_file.path);
        char *outPair = new char[oldLen + newLen + 2];
        strcpy(outPair, delta->new_file.path);
        outPair[newLen] = '\n';
        outPair[newLen + 1] = '\0';
        strcat(outPair, delta->old_file.path);

        if (isEqualNewFile) {
          std::pair<git_commit *, std::pair<char *, git_delta_t> > *historyEntry;
          if (!isEqualOldFile) {
            historyEntry = new std::pair<git_commit *, std::pair<char *, git_delta_t> >(
              nextCommit,
              std::pair<char *, git_delta_t>(strdup(outPair), delta->status)
            );
          } else {
            historyEntry = new std::pair<git_commit *, std::pair<char *, git_delta_t> >(
              nextCommit,
              std::pair<char *, git_delta_t>(strdup(delta->new_file.path), delta->status)
            );
          }
          baton->out->push_back(historyEntry);
          flag = true;
        } else if (isEqualOldFile) {
          std::pair<git_commit *, std::pair<char *, git_delta_t> > *historyEntry;
          historyEntry = new std::pair<git_commit *, std::pair<char *, git_delta_t> >(
            nextCommit,
            std::pair<char *, git_delta_t>(strdup(outPair), delta->status)
          );
          baton->out->push_back(historyEntry);
          flag = true;
        }

        delete[] outPair;

        git_patch_free(nextPatch);

        if (flag) {
          break;
        }
      }
    }

    git_diff_free(diffs);

    if (!flag && nextCommit != NULL) {
      git_commit_free(nextCommit);
    }

    if (baton->error_code != GIT_OK) {
      break;
    }
  }

  free(nextOid);

  if (baton->error_code != GIT_OK) {
    if (baton->error_code != GIT_ITEROVER) {
      baton->error = git_error_dup(giterr_last());

      while(!baton->out->empty())
      {
        std::pair<git_commit *, std::pair<char *, git_delta_t> > *pairToFree = baton->out->back();
        baton->out->pop_back();
        git_commit_free(pairToFree->first);
        free(pairToFree->second.first);
        free(pairToFree);
      }

      delete baton->out;

      baton->out = NULL;
    }
  } else {
    baton->error_code = GIT_OK;
  }
}

void GitRevwalk::FileHistoryWalkWorker::HandleOKCallback()
{
  if (baton->out != NULL) {
    unsigned int size = baton->out->size();
    Local<Array> result = Nan::New<Array>(size);
    for (unsigned int i = 0; i < size; i++) {
      Local<v8::Object> historyEntry = Nan::New<Object>();
      std::pair<git_commit *, std::pair<char *, git_delta_t> > *batonResult = baton->out->at(i);
      Nan::Set(historyEntry, Nan::New("commit").ToLocalChecked(), GitCommit::New(batonResult->first, true));
      Nan::Set(historyEntry, Nan::New("status").ToLocalChecked(), Nan::New<Number>(batonResult->second.second));
      if (batonResult->second.second == GIT_DELTA_RENAMED) {
        char *namePair = batonResult->second.first;
        char *split = strchr(namePair, '\n');
        *split = '\0';
        char *oldName = split + 1;

        Nan::Set(historyEntry, Nan::New("oldName").ToLocalChecked(), Nan::New(oldName).ToLocalChecked());
        Nan::Set(historyEntry, Nan::New("newName").ToLocalChecked(), Nan::New(namePair).ToLocalChecked());
      }
      Nan::Set(result, Nan::New<Number>(i), historyEntry);

      free(batonResult->second.first);
      free(batonResult);
    }

    Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv);

    delete baton->out;
    return;
  }

  if (baton->error) {
    Local<v8::Value> argv[1] = {
      Nan::Error(baton->error->message)
    };
    callback->Call(1, argv);
    if (baton->error->message)
    {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
    return;
  }

  if (baton->error_code < 0) {
    Local<v8::Object> err = Nan::Error("Method next has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv);
    return;
  }

  callback->Call(0, NULL);
}
