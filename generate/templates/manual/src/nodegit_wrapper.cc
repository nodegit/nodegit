template<typename Traits>
NodeGitWrapper<Traits>::NodeGitWrapper(typename Traits::cType *raw, bool selfFreeing, v8::Local<v8::Object> owner)
  : nodegitContext(nodegit::Context::GetCurrentContext()) {
  nodegitContext->LinkTrackerList(this);
  if (Traits::isSingleton) {
    ReferenceCounter::incrementCountForPointer((void *)raw);
    this->raw = raw;
  } else if (!owner.IsEmpty()) {
    // if we have an owner, it could mean 2 things:
    //  1. We are borrowed memory from another struct and should not be freed. We will keep a handle to the owner
    //     so that the owner isn't gc'd while we are using its memory.
    //  2. We are borrowed memory from another struct and can be duplicated, so we should duplicate
    //     and become selfFreeing.
    //  3. We are cached memory, potentially on the repo or config.
    //     Even though we have a handle in another objects cache, we are expected to call free,
    //     otherwise we are leaking memory. Cached objects are reference counted in libgit2, but will be leaked
    //     even if the cache is cleared if we haven't freed them. We will keep a handle on the owner, even though it
    //     is probably safe as we're reference counted. This should at worst just ensure that the cache owner is the
    //     last thing to be freed, and that is more safety than anything else.
    if (Traits::isDuplicable) {
      Traits::duplicate(&this->raw, raw);
      selfFreeing = true;
    } else {
      SetNativeOwners(owner);
      this->owner.Reset(owner);
      this->raw = raw;
    }
  } else {
    this->raw = raw;
  }
  this->selfFreeing = selfFreeing;

  if (selfFreeing) {
    SelfFreeingInstanceCount++;
  } else {
    NonSelfFreeingConstructedCount++;
  }
}

template<typename Traits>
NodeGitWrapper<Traits>::NodeGitWrapper(const char *error)
  : nodegitContext(nodegit::Context::GetCurrentContext()) {
  selfFreeing = false;
  raw = NULL;
  Nan::ThrowError(error);
}

template<typename Traits>
NodeGitWrapper<Traits>::~NodeGitWrapper() {
  Unlink();
  if (Traits::isFreeable && selfFreeing) {
    Traits::free(raw);
    SelfFreeingInstanceCount--;
    raw = NULL;
  }
  else if (!selfFreeing) {
    --NonSelfFreeingConstructedCount;
  }
}

template<typename Traits>
NAN_METHOD(NodeGitWrapper<Traits>::JSNewFunction) {
  cppClass * instance;

  if (info.Length() == 0 || !info[0]->IsExternal()) {
    Nan::TryCatch tryCatch;
    instance = new cppClass();
    // handle the case where the default constructor is not supported
    if(tryCatch.HasCaught()) {
      delete instance;
      tryCatch.ReThrow();
      return;
    }
  } else {
    instance = new cppClass(static_cast<cType *>(
      Local<External>::Cast(info[0])->Value()),
      Nan::To<bool>(info[1]).FromJust(),
      info.Length() >= 3 && !info[2].IsEmpty() && info[2]->IsObject() ? Nan::To<v8::Object>(info[2]).ToLocalChecked() : Local<v8::Object>()
    );
  }

  instance->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

template<typename Traits>
void NodeGitWrapper<Traits>::SetNativeOwners(v8::Local<v8::Object> owners) {
  assert(owners->IsArray() || owners->IsObject());
  Nan::HandleScope scope;
  std::unique_ptr< std::vector<nodegit::TrackerWrap*> > trackerOwners =
    std::make_unique< std::vector<nodegit::TrackerWrap*> >();

  if (owners->IsArray()) {
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    const v8::Local<v8::Array> ownersArray = owners.As<v8::Array>();
    const uint32_t numOwners = ownersArray->Length();

    for (uint32_t i = 0; i < numOwners; ++i) {
      v8::Local<v8::Value> value = ownersArray->Get(context, i).ToLocalChecked();
      const v8::Local<v8::Object> object = value.As<v8::Object>();
      Nan::ObjectWrap *objectWrap = Nan::ObjectWrap::Unwrap<Nan::ObjectWrap>(object);
      trackerOwners->push_back(static_cast<nodegit::TrackerWrap*>(objectWrap));
    }
  }
  else if (owners->IsObject()) {
    Nan::ObjectWrap *objectWrap = Nan::ObjectWrap::Unwrap<Nan::ObjectWrap>(owners);
    trackerOwners->push_back(static_cast<nodegit::TrackerWrap*>(objectWrap));
  }

  SetTrackerWrapOwners(std::move(trackerOwners));
}

template<typename Traits>
v8::Local<v8::Value> NodeGitWrapper<Traits>::New(const typename Traits::cType *raw, bool selfFreeing, v8::Local<v8::Object> owner) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[3] = { Nan::New<External>((void *)raw), Nan::New(selfFreeing), owner };
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  Local<Function> constructor_template = nodegitContext->GetFromPersistent(
    std::string(Traits::className()) + "::Template"
  ).As<Function>();
  return scope.Escape(
    Nan::NewInstance(
      constructor_template,
      owner.IsEmpty() ? 2 : 3, // passing an empty handle as part of the arguments causes a crash
      argv
    ).ToLocalChecked());
}

template<typename Traits>
v8::Local<v8::Value> NodeGitWrapper<Traits>::New(v8::Local<v8::Function> constructorTemplate, const typename Traits::cType *raw, bool selfFreeing, v8::Local<v8::Object> owner) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[3] = { Nan::New<External>((void *)raw), Nan::New(selfFreeing), owner };
  return scope.Escape(
    Nan::NewInstance(
      constructorTemplate,
      3,
      argv
    ).ToLocalChecked());
}

template<typename Traits>
v8::Local<v8::Value> NodeGitWrapper<Traits>::New(v8::Local<v8::Function> constructorTemplate, const typename Traits::cType *raw, bool selfFreeing) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[2] = { Nan::New<External>((void *)raw), Nan::New(selfFreeing) };
  return scope.Escape(
    Nan::NewInstance(
      constructorTemplate,
      2,
      argv
    ).ToLocalChecked());
}

template<typename Traits>
v8::Local<v8::Function> NodeGitWrapper<Traits>::GetTemplate() {
  return nodegit::Context::GetCurrentContext()->GetFromPersistent(
    std::string(Traits::className()) + "::Template"
  ).As<v8::Function>();
}

template<typename Traits>
typename Traits::cType *NodeGitWrapper<Traits>::GetValue() {
  return raw;
}

template<typename Traits>
void NodeGitWrapper<Traits>::ClearValue() {
  raw = NULL;
}

template<typename Traits>
thread_local int NodeGitWrapper<Traits>::SelfFreeingInstanceCount;

template<typename Traits>
thread_local int NodeGitWrapper<Traits>::NonSelfFreeingConstructedCount;

template<typename Traits>
NAN_METHOD(NodeGitWrapper<Traits>::GetSelfFreeingInstanceCount) {
  info.GetReturnValue().Set(SelfFreeingInstanceCount);
}

template<typename Traits>
NAN_METHOD(NodeGitWrapper<Traits>::GetNonSelfFreeingConstructedCount) {
  info.GetReturnValue().Set(NonSelfFreeingConstructedCount);
}

template<typename Traits>
void NodeGitWrapper<Traits>::InitializeTemplate(v8::Local<v8::FunctionTemplate> &tpl) {
  Nan::SetMethod(tpl, "getSelfFreeingInstanceCount", GetSelfFreeingInstanceCount);
  Nan::SetMethod(tpl, "getNonSelfFreeingConstructedCount", GetNonSelfFreeingConstructedCount);
}

template<typename Traits>
void NodeGitWrapper<Traits>::Reference() {
  Ref();
  for (auto &i : referenceCallbacks) {
    i.second();
  }
}

template<typename Traits>
void NodeGitWrapper<Traits>::Unreference() {
  Unref();
  for (auto &i : unreferenceCallbacks) {
    i.second();
  }
}

template<typename Traits>
void NodeGitWrapper<Traits>::AddReferenceCallbacks(size_t fieldIndex, std::function<void()> refCb, std::function<void()> unrefCb) {
  referenceCallbacks[fieldIndex] = refCb;
  unreferenceCallbacks[fieldIndex] = unrefCb;
}

template<typename Traits>
void NodeGitWrapper<Traits>::SaveCleanupHandle(std::shared_ptr<nodegit::CleanupHandle> cleanupHandle) {
  childCleanupVector.push_back(cleanupHandle);
}
