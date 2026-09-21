#include <napi.h>
#include <string.h>

extern "C" {
  #include <git2.h>
  #include <git2/sys/filter.h>
  #include <git2/sys/errors.h>
}

#include "../include/context.h"
#include "../include/functions/copy.h"
#include "../include/filter.h"
#include "../include/filter_registry.h"

#include <map>

using namespace std;

namespace nodegit {

  GitFilterRegistry::GitFilterRegistry(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<GitFilterRegistry>(info) {
    this->raw = static_cast<git_filter *>(info[0].As<Napi::External<void>>().Data());
  }

  GitFilterRegistry::~GitFilterRegistry() {
    if (this->raw != NULL) {
      delete this->raw;
    }
  }

  void GitFilterRegistry::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
    Napi::Env env = target.Env();
    Napi::HandleScope scope(env);

    Napi::Object filterRegistry = Napi::Object::New(env);

    filterRegistry.Set("register", Napi::Function::New(env, Register, "register"));
    filterRegistry.Set("unregister", Napi::Function::New(env, Unregister, "unregister"));

    target.Set("FilterRegistry", filterRegistry);
    nodegitContext->SaveToPersistent("FilterRegistry", filterRegistry);
    std::shared_ptr<nodegit::FilterRegistryCleanupHandles> filterRegistryCleanupHandles(new nodegit::FilterRegistryCleanupHandles);
    nodegitContext->SaveCleanupHandle("filterRegistry", filterRegistryCleanupHandles);
  }

  Napi::Value GitFilterRegistry::JSNewFunction(const Napi::CallbackInfo& info) {
    if (info.Length() == 0 || !info[0].IsExternal()) {
      Napi::Error::New(info.Env(), "A new GitFilterRegistry cannot be instantiated.").ThrowAsJavaScriptException();
      return info.Env().Undefined();
    }

    new GitFilterRegistry(info);

    return info.This().As<Napi::Object>();
  }

  Napi::Value GitFilterRegistry::New(void *raw) {
    Napi::Env env = nodegit::Context::GetCurrentContext()->Env();
    nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
    Napi::Function constructor_template = nodegitContext->GetFromPersistent("GitFilterRegistry::Template").As<Napi::Function>();
    return constructor_template.New({ Napi::External<void>::New(env, (void *)raw) });
  }

  git_filter *GitFilterRegistry::GetValue() {
    return this->raw;
  }

  void GitFilterRegistry::Reference() {
    Ref();
  }

  void GitFilterRegistry::Unreference() {
    Unref();
  }

  void GitFilterRegistry::AddReferenceCallbacks(size_t fieldIndex, std::function<void()> refCb, std::function<void()> unrefCb) {
  }

  Napi::Value GitFilterRegistry::Register(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() == 0 || !info[0].IsString()) {
      Napi::Error::New(env, "String name is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 1 || !info[1].IsObject()) {
      Napi::Error::New(env, "Filter filter is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 2 || !info[2].IsNumber()) {
      Napi::Error::New(env, "Number priority is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 3 || !info[3].IsFunction()) {
      Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
    std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;

    RegisterBaton *baton = new RegisterBaton();

    {
      auto conversionResult = ConfigurableGitFilter::fromJavascript(nodegitContext, info[1]);
      if (!conversionResult.result) {
        delete baton;
        Napi::Error::New(env, conversionResult.error).ThrowAsJavaScriptException();
        return env.Undefined();
      }

      auto convertedObject = conversionResult.result;
      cleanupHandles["filter"] = convertedObject;
      baton->filter = convertedObject->GetValue();
    }

    std::string nameStr = info[0].As<Napi::String>().Utf8Value();
    baton->name = (const char *)malloc(nameStr.length() + 1);
    memcpy((void *)baton->name, nameStr.c_str(), nameStr.length());
    memset((void *)(((char *)baton->name) + nameStr.length()), 0, 1);

    baton->error_code = GIT_OK;
    baton->priority = (int)info[2].As<Napi::Number>().Int32Value();

    Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[3].As<Napi::Function>()));
    RegisterWorker *worker = new RegisterWorker(baton, callback, cleanupHandles);

    worker->Reference("name", info[0]);
    worker->Reference("priority", info[2]);

    nodegitContext->QueueWorker(worker);
    return env.Undefined();
  }

  nodegit::LockMaster GitFilterRegistry::RegisterWorker::AcquireLocks() {
    return nodegit::LockMaster(true, baton->name, baton->filter);
  }

  void GitFilterRegistry::RegisterWorker::Execute() {
    git_error_clear();

    int result = git_filter_register(baton->name, baton->filter, baton->priority);
    baton->error_code = result;

    if (result != GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
      baton->error = git_error_dup(git_error_last());
    }
  }

  void GitFilterRegistry::RegisterWorker::HandleErrorCallback() {
    if (baton->error) {
      if (baton->error->message) {
        free((void *)baton->error->message);
      }

      free((void *)baton->error);
    }

    free((void *)baton->name);

    delete baton;
  }

  void GitFilterRegistry::RegisterWorker::HandleOKCallback() {
    Napi::Env env = GetAsyncResource()->Env();
    if (baton->error_code == GIT_OK) {
      static_pointer_cast<nodegit::FilterRegistryCleanupHandles>(
        nodegit::Context::GetCurrentContext()->GetCleanupHandle("filterRegistry")
      )->registeredFilters[baton->name] = cleanupHandles["filter"];

      Napi::Value result = Napi::Number::New(env, baton->error_code);
      Napi::Value argv[2] = {
        env.Null(),
        result
      };
      CallCallback(argv, 2);
    }
    else if (baton->error) {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method register has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "FilterRegistry.register"));
      Napi::Value argv[1] = {
        err
      };
      CallCallback(argv, 1);

      if (baton->error->message) {
        free((void *)baton->error->message);
      }
      free((void *)baton->error);
    }
    else if (baton->error_code < 0) {
      Napi::Object err = Napi::Error::New(env, "Method register has thrown an error.").Value();
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "FilterRegistry.register"));
      Napi::Value argv[1] = {
        err
      };
      CallCallback(argv, 1);
    }
    else {
      CallCallback(nullptr, 0);
    }

    free((void *)baton->name);

    delete baton;
  }

  Napi::Value GitFilterRegistry::Unregister(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() == 0 || !info[0].IsString()) {
      Napi::Error::New(env, "String name is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 1 || !info[1].IsFunction()) {
      Napi::Error::New(env, "Callback is required and must be a Function.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    UnregisterBaton *baton = new UnregisterBaton();

    std::string nameStr = info[0].As<Napi::String>().Utf8Value();
    baton->name = (const char *)malloc(nameStr.length() + 1);
    memcpy((void *)baton->name, nameStr.c_str(), nameStr.length());
    memset((void *)(((char *)baton->name) + nameStr.length()), 0, 1);

    baton->error_code = GIT_OK;

    Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[1].As<Napi::Function>()));
    UnregisterWorker *worker = new UnregisterWorker(baton, callback);

    nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
    nodegitContext->QueueWorker(worker);
    return env.Undefined();
  }

  nodegit::LockMaster GitFilterRegistry::UnregisterWorker::AcquireLocks() {
    return nodegit::LockMaster(true, baton->name);
  }

  void GitFilterRegistry::UnregisterWorker::Execute() {
    git_error_clear();

    int result = git_filter_unregister(baton->name);
    baton->error_code = result;

    if (result != GIT_OK && git_error_last()->klass != GIT_ERROR_NONE) {
      baton->error = git_error_dup(git_error_last());
    }
  }

  void GitFilterRegistry::UnregisterWorker::HandleErrorCallback() {
    if (baton->error) {
      if (baton->error->message) {
        free((void *)baton->error->message);
      }

      free((void *)baton->error);
    }

    free((void *)baton->name);

    delete baton;
  }

  void GitFilterRegistry::UnregisterWorker::HandleOKCallback() {
    Napi::Env env = GetAsyncResource()->Env();
    if (baton->error_code == GIT_OK) {
      static_pointer_cast<nodegit::FilterRegistryCleanupHandles>(
        nodegit::Context::GetCurrentContext()->GetCleanupHandle("filterRegistry")
      )->registeredFilters.erase(baton->name);

      Napi::Value result = Napi::Number::New(env, baton->error_code);
      Napi::Value argv[2] = {
        env.Null(),
        result
      };
      CallCallback(argv, 2);
    }
    else if (baton->error) {
      Napi::Object err;
      if (baton->error->message) {
        err = Napi::Error::New(env, baton->error->message).Value();
      } else {
        err = Napi::Error::New(env, "Method unregister has thrown an error.").Value();
      }
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "FilterRegistry.unregister"));
      Napi::Value argv[1] = {
        err
      };
      CallCallback(argv, 1);

      if (baton->error->message) {
        free((void *)baton->error->message);
      }
      free((void *)baton->error);
    }
    else if (baton->error_code < 0) {
      Napi::Object err = Napi::Error::New(env, "Method unregister has thrown an error.").Value();
      err.Set("errno", Napi::Number::New(env, baton->error_code));
      err.Set("errorFunction", Napi::String::New(env, "FilterRegistry.unregister"));
      Napi::Value argv[1] = {
        err
      };
      CallCallback(argv, 1);
    }
    else {
      CallCallback(nullptr, 0);
    }

    free((void *)baton->name);

    delete baton;
  }
}
