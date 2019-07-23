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

  int error = git_reference_resolve(out, ref);
  if (error != GIT_OK) {
    *out = NULL;
  }

  return GIT_OK;
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
    tagOdbBuffer(NULL),
    tagOdbBufferLength(0),
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

  static int fromReference(RefreshedRefModel **out, git_reference *ref, git_odb *odb) {
    RefreshedRefModel *refModel = new RefreshedRefModel(ref);
    const git_oid *referencedTargetOid = git_reference_target(ref);

    if (!git_reference_is_tag(ref)) {
      git_oid_tostr(refModel->sha, GIT_OID_HEXSZ + 1, referencedTargetOid);

      *out = refModel;
      return GIT_OK;
    }
    git_repository *repo = git_reference_owner(ref);

    git_tag *referencedTag;
    if (git_tag_lookup(&referencedTag, repo, referencedTargetOid) == GIT_OK) {
      refModel->message = strdup(git_tag_message(referencedTag));

      git_odb_object *tagOdbObject;
      if (git_odb_read(&tagOdbObject, odb, git_tag_id(referencedTag)) == GIT_OK) {
        refModel->tagOdbBufferLength = git_odb_object_size(tagOdbObject);
        refModel->tagOdbBuffer = new char[refModel->tagOdbBufferLength];
        std::memcpy(refModel->tagOdbBuffer, git_odb_object_data(tagOdbObject), refModel->tagOdbBufferLength);
        git_odb_object_free(tagOdbObject);
      }

      git_tag_free(referencedTag);
    }

    git_oid peeledReferencedTargetOid;
    int error = getOidOfReferenceCommit(&peeledReferencedTargetOid, ref);
    if (error != GIT_OK) {
      delete refModel;
      return error;
    }

    git_oid_tostr(refModel->sha, GIT_OID_HEXSZ + 1, &peeledReferencedTargetOid);

    *out = refModel;
    return GIT_OK;
  }

  static void ensureSignatureRegexes() {
    if (!signatureRegexesBySignatureType.IsEmpty()) {
      return;
    }

    v8::Local<v8::Array> gpgsigArray = Nan::New<v8::Array>(2),
      x509Array = Nan::New<v8::Array>(1);

    Nan::Set(
      gpgsigArray,
      Nan::New<Number>(0),
      Nan::New<v8::RegExp>(
        Nan::New("-----BEGIN PGP SIGNATURE-----[\\s\\S]+?-----END PGP SIGNATURE-----").ToLocalChecked(),
        static_cast<v8::RegExp::Flags>(v8::RegExp::Flags::kGlobal | v8::RegExp::Flags::kMultiline)
      ).ToLocalChecked()
    );

    Nan::Set(
      gpgsigArray,
      Nan::New<Number>(1),
      Nan::New<v8::RegExp>(
        Nan::New("-----BEGIN PGP MESSAGE-----[\\s\\S]+?-----END PGP MESSAGE-----").ToLocalChecked(),
        static_cast<v8::RegExp::Flags>(v8::RegExp::Flags::kGlobal | v8::RegExp::Flags::kMultiline)
      ).ToLocalChecked()
    );

    Nan::Set(
      x509Array,
      Nan::New<Number>(0),
      Nan::New<v8::RegExp>(
        Nan::New("-----BEGIN SIGNED MESSAGE-----[\\s\\S]+?-----END SIGNED MESSAGE-----").ToLocalChecked(),
        static_cast<v8::RegExp::Flags>(v8::RegExp::Flags::kGlobal | v8::RegExp::Flags::kMultiline)
      ).ToLocalChecked()
    );

    v8::Local<v8::Object> result = Nan::New<Object>();
    Nan::Set(result, Nan::New("gpgsig").ToLocalChecked(), gpgsigArray);
    Nan::Set(result, Nan::New("x509").ToLocalChecked(), x509Array);

    signatureRegexesBySignatureType.Reset(result);
  }

  v8::Local<v8::Object> toJavascript(v8::Local<v8::String> signatureType) {
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

    v8::Local<v8::Value> jsTagSignature = Nan::Null();
    if (tagOdbBuffer != NULL && tagOdbBufferLength != 0) {
      // tagOdbBuffer is already a copy, so we'd like to use NewBuffer instead,
      // but we were getting segfaults and couldn't easily figure out why. :(
      // We tried passing the tagOdbBuffer directly to NewBuffer and then nullifying tagOdbBuffer so that
      // the destructor didn't double free, but that still segfaulted internally in Node.
      v8::Local<v8::Object> buffer = Nan::CopyBuffer(tagOdbBuffer, tagOdbBufferLength).ToLocalChecked();
      v8::Local<v8::Value> toStringProp = Nan::Get(buffer, Nan::New("toString").ToLocalChecked()).ToLocalChecked();
      v8::Local<v8::Object> jsTagOdbObjectString = Nan::To<v8::Object>(Nan::CallAsFunction(Nan::To<v8::Object>(toStringProp).ToLocalChecked(), buffer, 0, NULL).ToLocalChecked()).ToLocalChecked();

      v8::Local<v8::Object> _signatureRegexesBySignatureType = Nan::New(signatureRegexesBySignatureType);
      v8::Local<v8::Array> signatureRegexes = v8::Local<v8::Array>::Cast(Nan::Get(_signatureRegexesBySignatureType, signatureType).ToLocalChecked());

      for (uint32_t i = 0; i < signatureRegexes->Length(); ++i) {
        v8::Local<v8::Value> argv[] = {
          Nan::Get(signatureRegexes, Nan::New(i)).ToLocalChecked()
        };

        v8::Local<v8::Value> matchProp = Nan::Get(jsTagOdbObjectString, Nan::New("match").ToLocalChecked()).ToLocalChecked();
        v8::Local<v8::Value> match = Nan::CallAsFunction(Nan::To<v8::Object>(matchProp).ToLocalChecked(), jsTagOdbObjectString, 1, argv).ToLocalChecked();
        if (match->IsArray()) {
          jsTagSignature = Nan::Get(Nan::To<v8::Object>(match).ToLocalChecked(), 0).ToLocalChecked();
          break;
        }
      }
    }
    Nan::Set(result, Nan::New("tagSignature").ToLocalChecked(), jsTagSignature);

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
    if (tagOdbBuffer != NULL) { delete[] tagOdbBuffer; }
  }

  char *fullName, *message, *sha, *shorthand, *tagOdbBuffer;
  size_t tagOdbBufferLength;
  const char *type;
  static Nan::Persistent<v8::Object> signatureRegexesBySignatureType;
};

Nan::Persistent<v8::Object> RefreshedRefModel::signatureRegexesBySignatureType;

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
  v8::Local<v8::String> signatureType;
  if (info.Length() == 2) {
    if (!info[0]->IsString()) {
      return Nan::ThrowError("Signature type must be \"gpgsig\" or \"x509\".");
    }

    v8::Local<v8::String> signatureTypeParam = Nan::To<v8::String>(info[0]).ToLocalChecked();
    if (
      Nan::Equals(signatureTypeParam, Nan::New("gpgsig").ToLocalChecked()) != Nan::Just(true)
      && Nan::Equals(signatureTypeParam, Nan::New("x509").ToLocalChecked()) != Nan::Just(true)
    ) {
      return Nan::ThrowError("Signature type must be \"gpgsig\" or \"x509\".");
    }
    signatureType = signatureTypeParam;
  } else {
    signatureType = Nan::New("gpgsig").ToLocalChecked();
  }

  if (info.Length() == 0 || (info.Length() == 1 && !info[0]->IsFunction()) || (info.Length() == 2 && !info[1]->IsFunction())) {
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
  worker->SaveToPersistent("signatureType", signatureType);
  Nan::AsyncQueueWorker(worker);
  return;
}

void GitRepository::RefreshReferencesWorker::Execute()
{
  giterr_clear();

  LockMaster lockMaster(true, baton->repo);
  git_repository *repo = baton->repo;
  RefreshReferencesData *refreshData = (RefreshReferencesData *)baton->out;
  git_odb *odb;

  baton->error_code = git_repository_odb(&odb, repo);
  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    delete refreshData;
    baton->out = NULL;
    return;
  }

  // START Refresh HEAD
  git_reference *headRef = NULL;
  baton->error_code = lookupDirectReferenceByShorthand(&headRef, repo, "HEAD");

  if (baton->error_code != GIT_OK || headRef == NULL) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    git_odb_free(odb);
    delete refreshData;
    baton->out = NULL;
    return;
  }

  RefreshedRefModel *headModel;
  baton->error_code = RefreshedRefModel::fromReference(&headModel, headRef, odb);
  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    git_odb_free(odb);
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
  if (lookupDirectReferenceByShorthand(&cherrypickRef, repo, "CHERRY_PICK_HEAD") == GIT_OK && cherrypickRef != NULL) {
    baton->error_code = RefreshedRefModel::fromReference(&refreshData->cherrypick, cherrypickRef, odb);
    git_reference_free(cherrypickRef);
  } else {
    cherrypickRef = NULL;
  }
  // END Refresh CHERRY_PICK_HEAD

  // START Refresh MERGE_HEAD
  git_reference *mergeRef = NULL;
  // fall through if cherry pick failed
  if (baton->error_code == GIT_OK && lookupDirectReferenceByShorthand(&mergeRef, repo, "MERGE_HEAD") == GIT_OK && mergeRef != NULL) {
    baton->error_code = RefreshedRefModel::fromReference(&refreshData->merge, mergeRef, odb);
    git_reference_free(mergeRef);
  } else {
    mergeRef = NULL;
  }
  // END Refresh MERGE_HEAD

  if (baton->error_code != GIT_OK) {
    if (giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
    git_odb_free(odb);
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
    git_odb_free(odb);
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
    git_odb_free(odb);
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
    if (reference == NULL) {
      // lookup found the reference but failed to resolve it directly
      continue;
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
    baton->error_code = RefreshedRefModel::fromReference(&refreshedRefModel, reference, odb);
    git_reference_free(reference);

    if (baton->error_code == GIT_OK) {
      refreshData->refs.push_back(refreshedRefModel);
    } else {
      baton->error_code = GIT_OK;
    }
  }

  git_odb_free(odb);
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
    RefreshedRefModel::ensureSignatureRegexes();
    RefreshReferencesData *refreshData = (RefreshReferencesData *)baton->out;
    v8::Local<v8::Object> result = Nan::New<Object>();

    Nan::Set(
      result,
      Nan::New("headRefFullName").ToLocalChecked(),
      Nan::New<String>(refreshData->headRefFullName).ToLocalChecked()
    );

    v8::Local<v8::String> signatureType = Nan::To<v8::String>(GetFromPersistent("signatureType")).ToLocalChecked();

    unsigned int numRefs = refreshData->refs.size();
    v8::Local<v8::Array> refs = Nan::New<v8::Array>(numRefs);
    for (unsigned int i = 0; i < numRefs; ++i) {
      RefreshedRefModel *refreshedRefModel = refreshData->refs[i];
      Nan::Set(refs, Nan::New(i), refreshedRefModel->toJavascript(signatureType));
    }
    Nan::Set(result, Nan::New("refs").ToLocalChecked(), refs);

    unsigned int numUpstreamInfo = refreshData->upstreamInfo.size();
    v8::Local<v8::Array> upstreamInfo = Nan::New<v8::Array>(numUpstreamInfo);
    for (unsigned int i = 0; i < numUpstreamInfo; ++i) {
      UpstreamModel *upstreamModel = refreshData->upstreamInfo[i];
      Nan::Set(upstreamInfo, Nan::New(i), upstreamModel->toJavascript());
    }
    Nan::Set(result, Nan::New("upstreamInfo").ToLocalChecked(), upstreamInfo);

    if (refreshData->cherrypick != NULL) {
      Nan::Set(
        result,
        Nan::New("cherrypick").ToLocalChecked(),
        refreshData->cherrypick->toJavascript(signatureType)
      );
    } else {
      Nan::Set(result, Nan::New("cherrypick").ToLocalChecked(), Nan::Null());
    }

    if (refreshData->merge != NULL) {
      Nan::Set(
        result,
        Nan::New("merge").ToLocalChecked(),
        refreshData->merge->toJavascript(signatureType)
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
    Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Repository refreshReferences has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("Repository.refreshReferences").ToLocalChecked());
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
