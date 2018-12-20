int getOidOfReferenceCommit(git_oid *commitOid, git_reference *ref) {
  git_object *commitObject;
  int result = git_reference_peel(&commitObject, ref, GIT_OBJ_COMMIT);

  if (result != GIT_OK) {
    return result;
  }

  git_oid_cpy(commitOid, git_object_id(commitObject));
  git_object_free(commitObject);
  return result;
}

int asDirectReference(git_reference **out, git_reference *ref) {
  if (git_reference_type(ref) != GIT_REF_SYMBOLIC) {
    return git_reference_dup(out, ref);
  }

  return git_reference_resolve(out, ref);
}

int lookupDirectReferenceByShorthand(git_reference **out, git_repository *repo, const char *shorthand) {
  git_reference *ref = NULL;
  int result = git_reference_dwim(&ref, repo, shorthand);

  if (result != GIT_OK) {
    return result;
  }

  result = asDirectReference(out, ref);
  git_reference_free(ref);
  return result;
}

int lookupDirectReferenceByFullName(git_reference **out, git_repository *repo, const char *fullName) {
  git_reference *ref = NULL;
  int result = git_reference_lookup(&ref, repo, fullName);

  if (result != GIT_OK) {
    return result;
  }

  result = asDirectReference(out, ref);
  git_reference_free(ref);
  return result;
}

char *getRemoteNameOfReference(git_reference *remoteReference) {
  return strtok(strdup(git_reference_shorthand(remoteReference)), "/");
}

bool gitStrArrayContains(git_strarray *strarray, const char *string) {
  for (size_t i = 0; i < strarray->count; ++i) {
    if (strcmp(strarray->strings[i], string) == 0) {
      return true;
    }
  }
  return false;
}

class RefreshedRefModel {
public:
  RefreshedRefModel(git_reference *ref):
    fullName(strdup(git_reference_name(ref))),
    message(NULL),
    sha(new char[GIT_OID_HEXSZ + 1]),
    shorthand(strdup(git_reference_shorthand(ref))),
    type(NULL)
  {
    if (git_reference_is_branch(ref)) {
      type = "branch";
    } else if (git_reference_is_remote(ref)) {
      type = "remote";
    } else {
      type = "tag";
    }
  }

  static int fromReference(RefreshedRefModel **out, git_reference *ref) {
    RefreshedRefModel *refModel = new RefreshedRefModel(ref);
    git_oid referencedTargetOid;

    int result = getOidOfReferenceCommit(&referencedTargetOid, ref);
    if (result != GIT_OK) {
      delete refModel;
      return result;
    }

    if (git_reference_is_tag(ref)) {
      git_repository *repo = git_reference_owner(ref);

      git_tag *referencedTag;
      if (git_tag_lookup(&referencedTag, repo, &referencedTargetOid) == GIT_OK) {
        refModel->message = strdup(git_tag_message(referencedTag));
        git_tag_free(referencedTag);
      }
    }

    git_oid_tostr(refModel->sha, GIT_OID_HEXSZ + 1, &referencedTargetOid);

    *out = refModel;
    return GIT_OK;
  }

  v8::Local<v8::Object> toJavascript() {
    v8::Local<v8::Object> result = Nan::New<Object>();

    v8::Local<v8::Value> jsFullName;
    if (fullName == NULL) {
      jsFullName = Nan::Null();
    } else {
      jsFullName = Nan::New<String>(fullName).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("fullName").ToLocalChecked(), jsFullName);

    v8::Local<v8::Value> jsMessage;
    if (message == NULL) {
      jsMessage = Nan::Null();
    } else {
      jsMessage = Nan::New<String>(message).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("message").ToLocalChecked(), jsMessage);

    Nan::Set(
      result,
      Nan::New("sha").ToLocalChecked(),
      Nan::New<String>(sha).ToLocalChecked()
    );

    v8::Local<v8::Value> jsShorthand;
    if (shorthand == NULL) {
      jsShorthand = Nan::Null();
    } else {
      jsShorthand = Nan::New<String>(shorthand).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("shorthand").ToLocalChecked(), jsShorthand);

    v8::Local<v8::Value> jsType;
    if (type == NULL) {
      jsType = Nan::Null();
    } else {
      jsType = Nan::New<String>(type).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("type").ToLocalChecked(), jsType);

    return result;
  }

  ~RefreshedRefModel() {
    if (fullName != NULL) { delete[] fullName; }
    if (message != NULL) { delete[] message; }
    delete[] sha;
    if (shorthand != NULL) { delete[] shorthand; }
  }

  char *fullName, *message, *sha, *shorthand;
  const char *type;
};

class UpstreamModel {
public:
  UpstreamModel(const char *inputDownstreamFullName, const char *inputUpstreamFullName):
    downstreamFullName((char *)strdup(inputDownstreamFullName)),
    upstreamFullName((char *)strdup(inputUpstreamFullName)),
    ahead(0),
    behind(0) {}

  static bool fromReference(UpstreamModel **out, git_reference *ref) {
    if (!git_reference_is_branch(ref)) {
      return false;
    }

    git_reference *upstream;
    int result = git_branch_upstream(&upstream, ref);
    if (result != GIT_OK) {
      return false;
    }

    UpstreamModel *upstreamModel = new UpstreamModel(
      git_reference_name(ref),
      git_reference_name(upstream)
    );

    git_oid localCommitOid;
    result = getOidOfReferenceCommit(&localCommitOid, ref);
    if (result != GIT_OK) {
      delete upstreamModel;
      return false;
    }

    git_oid upstreamCommitOid;
    result = getOidOfReferenceCommit(&upstreamCommitOid, upstream);
    if (result != GIT_OK) {
      delete upstreamModel;
      return false;
    }

    result = git_graph_ahead_behind(
      &upstreamModel->ahead,
      &upstreamModel->behind,
      git_reference_owner(ref),
      &localCommitOid,
      &upstreamCommitOid
    );

    if (result != GIT_OK) {
      delete upstreamModel;
      return false;
    }

    *out = upstreamModel;
    return true;
  }

  v8::Local<v8::Object> toJavascript() {
    v8::Local<v8::Object> result = Nan::New<Object>();

    v8::Local<v8::Value> jsDownstreamFullName;
    if (downstreamFullName == NULL) {
      jsDownstreamFullName = Nan::Null();
    } else {
      jsDownstreamFullName = Nan::New<String>(downstreamFullName).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("downstreamFullName").ToLocalChecked(), jsDownstreamFullName);

    v8::Local<v8::Value> jsUpstreamFullName;
    if (upstreamFullName == NULL) {
      jsUpstreamFullName = Nan::Null();
    } else {
      jsUpstreamFullName = Nan::New<String>(upstreamFullName).ToLocalChecked();
    }
    Nan::Set(result, Nan::New("upstreamFullName").ToLocalChecked(), jsUpstreamFullName);

    Nan::Set(result, Nan::New("ahead").ToLocalChecked(), Nan::New<Number>(ahead));
    Nan::Set(result, Nan::New("behind").ToLocalChecked(), Nan::New<Number>(behind));
    return result;
  }

  ~UpstreamModel() {
    if (downstreamFullName != NULL) { delete[] downstreamFullName; }
    if (upstreamFullName != NULL) { delete[] upstreamFullName; }
  }

  char *downstreamFullName;
  char *upstreamFullName;
  size_t ahead;
  size_t behind;
};

class RefreshReferencesData {
public:
  RefreshReferencesData():
    headRefFullName(NULL),
    cherrypick(NULL),
    merge(NULL) {}

  ~RefreshReferencesData() {
    while(refs.size()) {
      delete refs.back();
      refs.pop_back();
    }
    while(upstreamInfo.size()) {
      delete upstreamInfo.back();
      upstreamInfo.pop_back();
    }
    if (headRefFullName != NULL) { delete[] headRefFullName; }
    if (cherrypick != NULL) { delete cherrypick; }
    if (merge != NULL) { delete merge; }
  }

  std::vector<RefreshedRefModel *> refs;
  std::vector<UpstreamModel *> upstreamInfo;
  char *headRefFullName;
  RefreshedRefModel *cherrypick;
  RefreshedRefModel *merge;
};

NAN_METHOD(GitRepository::RefreshReferences)
{
  if (info.Length() == 0 || !info[0]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  RefreshReferencesBaton* baton = new RefreshReferencesBaton;

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (void *)new RefreshReferencesData;
  baton->repo = Nan::ObjectWrap::Unwrap<GitRepository>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[0]));
  RefreshReferencesWorker *worker = new RefreshReferencesWorker(baton, callback);
  worker->SaveToPersistent("repo", info.This());
  Nan::AsyncQueueWorker(worker);
  return;
}

void GitRepository::RefreshReferencesWorker::Execute()
{
  giterr_clear();

  LockMaster lockMaster(true, baton->repo);
  git_repository *repo = baton->repo;
  RefreshReferencesData *refreshData = (RefreshReferencesData *)baton->out;

  // START Refresh HEAD
  git_reference *headRef = NULL;
  baton->error_code = lookupDirectReferenceByShorthand(&headRef, repo, "HEAD");

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete refreshData;
    baton->out = NULL;
    return;
  }

  RefreshedRefModel *headModel;
  baton->error_code = RefreshedRefModel::fromReference(&headModel, headRef);
  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    git_reference_free(headRef);
    delete refreshData;
    baton->out = NULL;
    return;
  }
  refreshData->refs.push_back(headModel);

  refreshData->headRefFullName = strdup(git_reference_name(headRef));
  git_reference_free(headRef);
  // END Refresh HEAD

  // START Refresh CHERRY_PICK_HEAD
  git_reference *cherrypickRef = NULL;
  if (lookupDirectReferenceByShorthand(&cherrypickRef, repo, "CHERRY_PICK_HEAD") == GIT_OK) {
    baton->error_code = RefreshedRefModel::fromReference(&refreshData->cherrypick, cherrypickRef);
    git_reference_free(cherrypickRef);
  } else {
    cherrypickRef = NULL;
  }
  // END Refresh CHERRY_PICK_HEAD

  // START Refresh MERGE_HEAD
  git_reference *mergeRef = NULL;
  // fall through if cherry pick failed
  if (baton->error_code == GIT_OK && lookupDirectReferenceByShorthand(&mergeRef, repo, "MERGE_HEAD") == GIT_OK) {
    baton->error_code = RefreshedRefModel::fromReference(&refreshData->merge, mergeRef);
    git_reference_free(mergeRef);
  } else {
    mergeRef = NULL;
  }
  // END Refresh MERGE_HEAD

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete refreshData;
    baton->out = NULL;
    return;
  }

  // Retrieve reference models and upstream info for each reference
  git_strarray referenceNames;
  baton->error_code = git_reference_list(&referenceNames, repo);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete refreshData;
    baton->out = NULL;
    return;
  }

  git_strarray remoteNames;
  baton->error_code = git_remote_list(&remoteNames, repo);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    git_strarray_free(&referenceNames);
    delete refreshData;
    baton->out = NULL;
    return;
  }

  for (size_t referenceIndex = 0; referenceIndex < referenceNames.count; ++referenceIndex) {
    git_reference *reference;
    baton->error_code = lookupDirectReferenceByFullName(&reference, repo, referenceNames.strings[referenceIndex]);

    if (baton->error_code != GIT_OK) {
      break;
    }

    UpstreamModel *upstreamModel;
    if (UpstreamModel::fromReference(&upstreamModel, reference)) {
      refreshData->upstreamInfo.push_back(upstreamModel);
    }

    bool isBranch = git_reference_is_branch(reference);
    bool isRemote = git_reference_is_remote(reference);
    bool isTag = git_reference_is_tag(reference);
    if (
      strcmp(referenceNames.strings[referenceIndex], headModel->fullName) == 0
      || (!isBranch && !isRemote && !isTag)
    ) {
      git_reference_free(reference);
      continue;
    }

    if (isRemote) {
      char *remoteNameOfRef = getRemoteNameOfReference(reference);
      bool isFromExistingRemote = gitStrArrayContains(&remoteNames, remoteNameOfRef);
      delete[] remoteNameOfRef;
      if (!isFromExistingRemote) {
        git_reference_free(reference);
        continue;
      }
    }

    RefreshedRefModel *refreshedRefModel;
    baton->error_code = RefreshedRefModel::fromReference(&refreshedRefModel, reference);
    git_reference_free(reference);

    if (baton->error_code == GIT_OK) {
      refreshData->refs.push_back(refreshedRefModel);
    }
  }

  git_strarray_free(&remoteNames);
  git_strarray_free(&referenceNames);

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete refreshData;
    baton->out = NULL;
    return;
  }
}

void GitRepository::RefreshReferencesWorker::HandleOKCallback()
{
  if (baton->out != NULL)
  {
    RefreshReferencesData *refreshData = (RefreshReferencesData *)baton->out;
    v8::Local<v8::Object> result = Nan::New<Object>();

    Nan::Set(
      result,
      Nan::New("headRefFullName").ToLocalChecked(),
      Nan::New<String>(refreshData->headRefFullName).ToLocalChecked()
    );

    unsigned int numRefs = refreshData->refs.size();
    v8::Local<v8::Array> refs = Nan::New<Array>(numRefs);
    for (unsigned int i = 0; i < numRefs; ++i) {
      RefreshedRefModel *refreshedRefModel = refreshData->refs[i];
      Nan::Set(refs, Nan::New<Number>(i), refreshedRefModel->toJavascript());
    }
    Nan::Set(result, Nan::New("refs").ToLocalChecked(), refs);

    unsigned int numUpstreamInfo = refreshData->upstreamInfo.size();
    v8::Local<v8::Array> upstreamInfo = Nan::New<Array>(numUpstreamInfo);
    for (unsigned int i = 0; i < numUpstreamInfo; ++i) {
      UpstreamModel *upstreamModel = refreshData->upstreamInfo[i];
      Nan::Set(upstreamInfo, Nan::New<Number>(i), upstreamModel->toJavascript());
    }
    Nan::Set(result, Nan::New("upstreamInfo").ToLocalChecked(), upstreamInfo);

    if (refreshData->cherrypick != NULL) {
      Nan::Set(
        result,
        Nan::New("cherrypick").ToLocalChecked(),
        refreshData->cherrypick->toJavascript()
      );
    } else {
      Nan::Set(result, Nan::New("cherrypick").ToLocalChecked(), Nan::Null());
    }

    if (refreshData->merge != NULL) {
      Nan::Set(
        result,
        Nan::New("merge").ToLocalChecked(),
        refreshData->merge->toJavascript()
      );
    } else {
      Nan::Set(result, Nan::New("merge").ToLocalChecked(), Nan::Null());
    }

    delete refreshData;

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
    Local<v8::Object> err = Nan::Error("Repository refreshReferences has thrown an error.")->ToObject();
    err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    err->Set(Nan::New("errorFunction").ToLocalChecked(), Nan::New("Repository.refreshReferences").ToLocalChecked());
    Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else
  {
    callback->Call(0, NULL, async_resource);
  }
}
