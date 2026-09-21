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

  RefreshedRefModel(const RefreshedRefModel &) = delete;
  RefreshedRefModel(RefreshedRefModel &&) = delete;
  RefreshedRefModel &operator=(const RefreshedRefModel &) = delete;
  RefreshedRefModel &operator=(RefreshedRefModel &&) = delete;

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
      const char *tagMessage = git_tag_message(referencedTag);
      refModel->message = tagMessage ? strdup(tagMessage) : NULL;

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

  static void ensureSignatureRegexes(Napi::Env env) {
    if (!signatureRegexesBySignatureType.IsEmpty()) {
      return;
    }

    Napi::Function regexpCtor = env.Global().Get("RegExp").As<Napi::Function>();

    Napi::Array gpgsigArray = Napi::Array::New(env, 2),
      x509Array = Napi::Array::New(env, 1);

    gpgsigArray.Set((uint32_t)0,
      regexpCtor.New({
        Napi::String::New(env, "-----BEGIN PGP SIGNATURE-----[\\s\\S]+?-----END PGP SIGNATURE-----"),
        Napi::String::New(env, "gm")
      })
    );

    gpgsigArray.Set((uint32_t)1,
      regexpCtor.New({
        Napi::String::New(env, "-----BEGIN PGP MESSAGE-----[\\s\\S]+?-----END PGP MESSAGE-----"),
        Napi::String::New(env, "gm")
      })
    );

    x509Array.Set((uint32_t)0,
      regexpCtor.New({
        Napi::String::New(env, "-----BEGIN SIGNED MESSAGE-----[\\s\\S]+?-----END SIGNED MESSAGE-----"),
        Napi::String::New(env, "gm")
      })
    );

    Napi::Object result = Napi::Object::New(env);
    result.Set("gpgsig", gpgsigArray);
    result.Set("x509", x509Array);

    signatureRegexesBySignatureType = Napi::Persistent(result);
  }

  Napi::Value toJavascript(Napi::Env env, Napi::Value signatureType) {
    Napi::Object result = Napi::Object::New(env);

    Napi::Value jsFullName;
    if (fullName == NULL) {
      jsFullName = env.Null();
    } else {
      jsFullName = Napi::String::New(env, fullName);
    }
    result.Set("fullName", jsFullName);

    Napi::Value jsMessage;
    if (message == NULL) {
      jsMessage = env.Null();
    } else {
      jsMessage = Napi::String::New(env, message);
    }
    result.Set("message", jsMessage);

    result.Set(
      "sha",
      Napi::String::New(env, sha)
    );

    Napi::Value jsShorthand;
    if (shorthand == NULL) {
      jsShorthand = env.Null();
    } else {
      jsShorthand = Napi::String::New(env, shorthand);
    }
    result.Set("shorthand", jsShorthand);

    Napi::Value jsTagSignature = env.Null();
    if (tagOdbBuffer != NULL && tagOdbBufferLength != 0) {
      // tagOdbBuffer is already a copy, so we'd like to use a non-copying buffer instead,
      // but we were getting segfaults and couldn't easily figure out why. :(
      Napi::Object buffer = Napi::Buffer<char>::Copy(env, tagOdbBuffer, tagOdbBufferLength).As<Napi::Object>();
      Napi::Value toStringProp = buffer.Get("toString");
      Napi::Object jsTagOdbObjectString = toStringProp.As<Napi::Function>().Call(buffer, std::vector<napi_value>()).As<Napi::Object>();

      Napi::Object _signatureRegexesBySignatureType = signatureRegexesBySignatureType.Value();
      Napi::Array signatureRegexes = _signatureRegexesBySignatureType.Get(signatureType).As<Napi::Array>();

      for (uint32_t i = 0; i < signatureRegexes.Length(); ++i) {
        std::vector<napi_value> argv;
        argv.push_back(signatureRegexes.Get(i));

        Napi::Value matchProp = jsTagOdbObjectString.Get("match");
        Napi::Value match = matchProp.As<Napi::Function>().Call(jsTagOdbObjectString, argv);
        if (match.IsArray()) {
          jsTagSignature = match.As<Napi::Object>().Get((uint32_t)0);
          break;
        }
      }
    }
    result.Set("tagSignature", jsTagSignature);

    Napi::Value jsType;
    if (type == NULL) {
      jsType = env.Null();
    } else {
      jsType = Napi::String::New(env, type);
    }
    result.Set("type", jsType);

    return result;
  }

  ~RefreshedRefModel() {
    if (fullName != NULL) { free(fullName); }
    if (message != NULL) { free(message); }
    delete[] sha;
    if (shorthand != NULL) { free(shorthand); }
    if (tagOdbBuffer != NULL) { delete[] tagOdbBuffer; }
  }

  char *fullName, *message, *sha, *shorthand, *tagOdbBuffer;
  size_t tagOdbBufferLength;
  const char *type;
  static Napi::ObjectReference signatureRegexesBySignatureType;
};

Napi::ObjectReference RefreshedRefModel::signatureRegexesBySignatureType;

class UpstreamModel {
public:
  UpstreamModel(const char *inputDownstreamFullName, const char *inputUpstreamFullName):
    downstreamFullName((char *)strdup(inputDownstreamFullName)),
    upstreamFullName((char *)strdup(inputUpstreamFullName)),
    ahead(0),
    behind(0) {}

  UpstreamModel(const UpstreamModel &) = delete;
  UpstreamModel(UpstreamModel &&) = delete;
  UpstreamModel &operator=(const UpstreamModel &) = delete;
  UpstreamModel &operator=(UpstreamModel &&) = delete;

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

  Napi::Value toJavascript(Napi::Env env) {
    Napi::Object result = Napi::Object::New(env);

    Napi::Value jsDownstreamFullName;
    if (downstreamFullName == NULL) {
      jsDownstreamFullName = env.Null();
    } else {
      jsDownstreamFullName = Napi::String::New(env, downstreamFullName);
    }
    result.Set("downstreamFullName", jsDownstreamFullName);

    Napi::Value jsUpstreamFullName;
    if (upstreamFullName == NULL) {
      jsUpstreamFullName = env.Null();
    } else {
      jsUpstreamFullName = Napi::String::New(env, upstreamFullName);
    }
    result.Set("upstreamFullName", jsUpstreamFullName);

    result.Set("ahead", Napi::Number::New(env, ahead));
    result.Set("behind", Napi::Number::New(env, behind));
    return result;
  }

  ~UpstreamModel() {
    if (downstreamFullName != NULL) { free(downstreamFullName); }
    if (upstreamFullName != NULL) { free(upstreamFullName); }
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

  RefreshReferencesData(const RefreshReferencesData &) = delete;
  RefreshReferencesData(RefreshReferencesData &&) = delete;
  RefreshReferencesData &operator=(const RefreshReferencesData &) = delete;
  RefreshReferencesData &operator=(RefreshReferencesData &&) = delete;

  ~RefreshReferencesData() {
    while(refs.size()) {
      delete refs.back();
      refs.pop_back();
    }
    while(upstreamInfo.size()) {
      delete upstreamInfo.back();
      upstreamInfo.pop_back();
    }
    if (headRefFullName != NULL) { free(headRefFullName); }
    if (cherrypick != NULL) { delete cherrypick; }
    if (merge != NULL) { delete merge; }
  }

  std::vector<RefreshedRefModel *> refs;
  std::vector<UpstreamModel *> upstreamInfo;
  char *headRefFullName;
  RefreshedRefModel *cherrypick;
  RefreshedRefModel *merge;
};

Napi::Value GitRepository::RefreshReferences(const Napi::CallbackInfo& info)
{
  Napi::Env env = info.Env();
  Napi::Value signatureType;
  if (info.Length() == 2) {
    if (!info[0].IsString()) {
      Napi::Error::New(env, "Signature type must be \"gpgsig\" or \"x509\".").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::String signatureTypeParam = info[0].As<Napi::String>();
    std::string signatureTypeStr = signatureTypeParam.Utf8Value();
    if (signatureTypeStr != "gpgsig" && signatureTypeStr != "x509") {
      Napi::Error::New(env, "Signature type must be \"gpgsig\" or \"x509\".").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    signatureType = signatureTypeParam;
  } else {
    signatureType = Napi::String::New(env, "gpgsig");
  }

  if (!info[info.Length() - 1].IsFunction()) {
    Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  RefreshReferencesBaton* baton = new RefreshReferencesBaton();

  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (void *)new RefreshReferencesData();
  baton->repo = NodeGitWrapper<GitRepositoryTraits>::Unwrap<GitRepository>(info.This().As<Napi::Object>())->GetValue();

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[info.Length() - 1].As<Napi::Function>()));
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;
  RefreshReferencesWorker *worker = new RefreshReferencesWorker(baton, callback, cleanupHandles);
  worker->Reference<GitRepository>("repo", info.This().As<Napi::Object>());
  worker->Reference("signatureType", signatureType);
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return env.Undefined();
}

nodegit::LockMaster GitRepository::RefreshReferencesWorker::AcquireLocks() {
  nodegit::LockMaster lockMaster(true, baton->repo);
  return lockMaster;
}

void GitRepository::RefreshReferencesWorker::Execute()
{
  giterr_clear();

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
      free(remoteNameOfRef);
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

void GitRepository::RefreshReferencesWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  RefreshReferencesData *refreshData = (RefreshReferencesData *)baton->out;
  delete refreshData;

  delete baton;
}

void GitRepository::RefreshReferencesWorker::HandleOKCallback()
{
  Napi::Env env = GetAsyncResource()->Env();
  if (baton->out != NULL)
  {
    RefreshedRefModel::ensureSignatureRegexes(env);
    auto refreshData = (RefreshReferencesData *)baton->out;
    Napi::Object result = Napi::Object::New(env);

    result.Set(
      "headRefFullName",
      Napi::String::New(env, refreshData->headRefFullName)
    );

    Napi::Value signatureType = GetFromPersistent("signatureType");

    unsigned int numRefs = refreshData->refs.size();
    Napi::Array refs = Napi::Array::New(env, numRefs);
    for (unsigned int i = 0; i < numRefs; ++i) {
      RefreshedRefModel *refreshedRefModel = refreshData->refs[i];
      refs.Set(i, refreshedRefModel->toJavascript(env, signatureType));
    }
    result.Set("refs", refs);

    unsigned int numUpstreamInfo = refreshData->upstreamInfo.size();
    Napi::Array upstreamInfo = Napi::Array::New(env, numUpstreamInfo);
    for (unsigned int i = 0; i < numUpstreamInfo; ++i) {
      UpstreamModel *upstreamModel = refreshData->upstreamInfo[i];
      upstreamInfo.Set(i, upstreamModel->toJavascript(env));
    }
    result.Set("upstreamInfo", upstreamInfo);

    if (refreshData->cherrypick != NULL) {
      result.Set(
        "cherrypick",
        refreshData->cherrypick->toJavascript(env, signatureType)
      );
    } else {
      result.Set("cherrypick", env.Null());
    }

    if (refreshData->merge != NULL) {
      result.Set(
        "merge",
        refreshData->merge->toJavascript(env, signatureType)
      );
    } else {
      result.Set("merge", env.Null());
    }

    delete refreshData;

    Napi::Value argv[2] = {
      env.Null(),
      result
    };
    CallCallback(argv, 2);
  }
  else if (baton->error)
  {
    Napi::Value argv[1] = {
      Napi::Error::New(env, baton->error->message).Value()
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
    Napi::Object err = Napi::Error::New(env, "Repository refreshReferences has thrown an error.").Value();
    err.Set("errno", Napi::Number::New(env, baton->error_code));
    err.Set("errorFunction", Napi::String::New(env, "Repository.refreshReferences"));
    Napi::Value argv[1] = {
      err
    };
    CallCallback(argv, 1);
  }
  else
  {
    CallCallback(nullptr, 0);
  }

  delete baton;
}
