template<typename Traits>
NodeGitWrapper<Traits>::NodeGitWrapper(typename Traits::cType *raw, bool selfFreeing, v8::Local<v8::Object> owner) {
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
NodeGitWrapper<Traits>::NodeGitWrapper(const char *error) {
  selfFreeing = false;
  raw = NULL;
  Nan::ThrowError(error);
}

template<typename Traits>
NodeGitWrapper<Traits>::~NodeGitWrapper() {
  if (Traits::isFreeable && selfFreeing) {
    Traits::free(raw);
    SelfFreeingInstanceCount--;
    raw = NULL;
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
      info.Length() >= 3 && !info[2].IsEmpty() && info[2]->IsObject() ? info[2]->ToObject() : Local<v8::Object>()
    );
  }

  instance->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

template<typename Traits>
v8::Local<v8::Value> NodeGitWrapper<Traits>::New(const typename Traits::cType *raw, bool selfFreeing, v8::Local<v8::Object> owner) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[3] = { Nan::New<External>((void *)raw), Nan::New(selfFreeing), owner };
  return scope.Escape(
    Nan::NewInstance(
      Nan::New(constructor_template),
      owner.IsEmpty() ? 2 : 3, // passing an empty handle as part of the arguments causes a crash
      argv
    ).ToLocalChecked());
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
Nan::Persistent<v8::Function> NodeGitWrapper<Traits>::constructor_template;

template<typename Traits>
int NodeGitWrapper<Traits>::SelfFreeingInstanceCount;

template<typename Traits>
int NodeGitWrapper<Traits>::NonSelfFreeingConstructedCount;

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
