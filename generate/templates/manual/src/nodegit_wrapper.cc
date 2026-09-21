#include "../include/nodegit_wrapper.h"

#include "../include/context.h"
#include "../include/reference_counter.h"

#include <assert.h>

template<typename Traits>
NodeGitWrapper<Traits>::NodeGitWrapper(const Napi::CallbackInfo& info)
  : nodegit::TrackerWrap(info), nodegitContext(nodegit::Context::GetCurrentContext()) {
  nodegitContext->LinkTrackerList(this);

  if (info.Length() > 0 && info[0].IsExternal()) {
    cType *raw = static_cast<cType*>(info[0].As<Napi::External<void>>().Data());
    bool selfFreeing = info[1].ToBoolean().Value();
    Napi::Object owner = (info.Length() >= 3 && info[2].IsObject())
      ? info[2].As<Napi::Object>()
      : Napi::Object();
    InitializeFromRaw(raw, selfFreeing, owner);
  } else {
    // Default construction. Structs allocate an empty cType; classes throw a
    // JS error (and DefaultAllocate returns nullptr).
    cType *raw = Traits::cppClass::DefaultAllocate(info.Env());
    this->raw = raw;
    this->selfFreeing = true;
  }

  if (this->selfFreeing) {
    SelfFreeingInstanceCount++;
  } else {
    NonSelfFreeingConstructedCount++;
  }
}

template<typename Traits>
void NodeGitWrapper<Traits>::InitializeFromRaw(cType *raw, bool selfFreeing, Napi::Object owner) {
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
      this->owner = std::make_unique<Napi::ObjectReference>(Napi::Persistent(owner));
      this->owner->SuppressDestruct();
      this->raw = raw;
    }
  } else {
    this->raw = raw;
  }
  this->selfFreeing = selfFreeing;
}

template<typename Traits>
NodeGitWrapper<Traits>::~NodeGitWrapper() {
  if (owner) {
    owner->SuppressDestruct();
    owner.reset();
  }
  Unlink();
  if (Traits::isFreeable && selfFreeing) {
    Traits::free(raw);
    SelfFreeingInstanceCount--;
    raw = NULL;
  }
  else if (!selfFreeing && raw != NULL) {
    --NonSelfFreeingConstructedCount;
    raw = NULL;
  }
}

template<typename Traits>
void NodeGitWrapper<Traits>::DestroyNative() {
  // Native resource teardown is handled by the destructor above via
  // Traits::free. This override exists so TrackerWrap::DestroyNative() can be
  // called uniformly (e.g. from reference-counted unlink paths); it is a no-op.
}

template<typename Traits>
Napi::Value NodeGitWrapper<Traits>::JSNewFunction(const Napi::CallbackInfo& info) {
  // The branching logic lives in the cppClass(const Napi::CallbackInfo&) ctor,
  // which forwards to NodeGitWrapper<Traits>(info). We just construct and wrap.
  new cppClass(info);
  return info.This().As<Napi::Object>();
}

template<typename Traits>
void NodeGitWrapper<Traits>::SetNativeOwners(Napi::Object owners) {
  assert(owners.IsArray() || owners.IsObject());
  Napi::HandleScope scope(owners.Env());
  std::unique_ptr< std::vector<nodegit::TrackerWrap*> > trackerOwners =
    std::unique_ptr< std::vector<nodegit::TrackerWrap*> >(new std::vector<nodegit::TrackerWrap*>());

  if (owners.IsArray()) {
    Napi::Array ownersArray = owners.As<Napi::Array>();
    const uint32_t numOwners = ownersArray.Length();

    for (uint32_t i = 0; i < numOwners; ++i) {
      Napi::Value value = ownersArray.Get(i);
      Napi::Object object = value.As<Napi::Object>();
      nodegit::TrackerWrap *objectWrap = Napi::ObjectWrap<nodegit::TrackerWrap>::Unwrap(object);
      trackerOwners->push_back(objectWrap);
    }
  }
  else if (owners.IsObject()) {
    nodegit::TrackerWrap *objectWrap = Napi::ObjectWrap<nodegit::TrackerWrap>::Unwrap(owners);
    trackerOwners->push_back(objectWrap);
  }

  SetTrackerWrapOwners(std::move(trackerOwners));
}

template<typename Traits>
Napi::Value NodeGitWrapper<Traits>::New(const cType *raw, bool selfFreeing, Napi::Object owner, nodegit::Context *nodegitContext) {
  if (!nodegitContext) {
    nodegitContext = nodegit::Context::GetCurrentContext();
  }
  if (!nodegitContext) {
    return Napi::Value();
  }
  Napi::Env env = nodegitContext->Env();
  if (!raw || env.IsExceptionPending()) {
    return env.Null();
  }
  Napi::EscapableHandleScope scope(env);

  Napi::Value tmplVal = nodegitContext->GetFromPersistent(std::string(Traits::className()) + "::Template");
  if (tmplVal.IsEmpty() || !tmplVal.IsFunction() || env.IsExceptionPending()) {
    return env.Null();
  }

  std::vector<napi_value> argv;
  argv.push_back(Napi::External<void>::New(env, (void *)raw));
  argv.push_back(Napi::Boolean::New(env, selfFreeing));
  if (!owner.IsEmpty()) {
    argv.push_back(owner);
  }

  napi_value result = nullptr;
  napi_status status = napi_new_instance(env, tmplVal.As<Napi::Function>(), argv.size(), argv.data(), &result);
  if (status != napi_ok || !result) {
    return env.Null();
  }
  return scope.Escape(Napi::Value(env, result));
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
Napi::Value NodeGitWrapper<Traits>::GetSelfFreeingInstanceCount(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), SelfFreeingInstanceCount);
}

template<typename Traits>
Napi::Value NodeGitWrapper<Traits>::GetNonSelfFreeingConstructedCount(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), NonSelfFreeingConstructedCount);
}

template<typename Traits>
void NodeGitWrapper<Traits>::InitializeTemplate(Napi::Object tpl) {
  Napi::Env env = tpl.Env();
  tpl.Set("getSelfFreeingInstanceCount", Napi::Function::New(env, GetSelfFreeingInstanceCount));
  tpl.Set("getNonSelfFreeingConstructedCount", Napi::Function::New(env, GetNonSelfFreeingConstructedCount));
}

template<typename Traits>
void NodeGitWrapper<Traits>::Reference() {
  Ref();
  for (auto &i : referenceCallbacks) {
    if (i.second) {
      i.second();
    }
  }
}

template<typename Traits>
void NodeGitWrapper<Traits>::Unreference() {
  Unref();
  for (auto &i : unreferenceCallbacks) {
    if (i.second) {
      i.second();
    }
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
