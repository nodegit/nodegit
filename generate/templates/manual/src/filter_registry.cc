#include <nan.h>
#include <string.h>

extern "C" {
  #include <git2.h>
}

#include "../include/nodegit.h"
#include "../include/cleanup_handle.h"
#include "../include/context.h"
#include "../include/lock_master.h"
#include "../include/functions/copy.h"
#include "../include/filter_registry.h"
#include "nodegit_wrapper.cc"

#include "../include/filter.h"

using namespace std;
using namespace v8;
using namespace node;

void GitFilterRegistry::InitializeComponent(v8::Local<v8::Object> target, nodegit::Context *nodegitContext) {
  Nan::HandleScope scope;

  v8::Local<Object> filterRegistry = Nan::New<Object>();

  Local<External> nodegitExternal = Nan::New<External>(nodegitContext);
  Nan::SetMethod(filterRegistry, "register", GitFilterRegister, nodegitExternal);
  Nan::SetMethod(filterRegistry, "unregister", GitFilterUnregister, nodegitExternal);

  Nan::Set(target, Nan::New<String>("FilterRegistry").ToLocalChecked(), filterRegistry);
  nodegitContext->SaveToPersistent("FilterRegistry", filterRegistry);
  std::shared_ptr<nodegit::FilterRegistryCleanupHandles> filterRegistryCleanupHandles(new nodegit::FilterRegistryCleanupHandles);
  nodegitContext->SaveCleanupHandle("filterRegistry", filterRegistryCleanupHandles);
}

NAN_METHOD(GitFilterRegistry::GitFilterRegister) {
  Nan::EscapableHandleScope scope;

  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("String name is required.");
  }

  if (info.Length() == 1 || !info[1]->IsObject()) {
    return Nan::ThrowError("Filter filter is required.");
  }

  if (info.Length() == 2 || !info[2]->IsNumber()) {
    return Nan::ThrowError("Number priority is required.");
  }

  if (info.Length() == 3 || !info[3]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FilterRegisterBaton *baton = new FilterRegisterBaton();
  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> cleanupHandles;

  {
    auto conversionResult = ConfigurableGitFilter::fromJavascript(nodegitContext, info[1]);
    if (!conversionResult.result) {
      delete baton;
      return Nan::ThrowError(Nan::New(conversionResult.error).ToLocalChecked());
    }

    auto convertedObject = conversionResult.result;
    cleanupHandles["filter"] = convertedObject;
    baton->filter = convertedObject->GetValue();
  }

  Nan::Utf8String name(Nan::To<v8::String>(info[0]).ToLocalChecked());

  baton->filter_name = (char *)malloc(name.length() + 1);
  memcpy((void *)baton->filter_name, *name, name.length());
  memset((void *)(((char *)baton->filter_name) + name.length()), 0, 1);

  baton->error_code = GIT_OK;
  baton->filter_priority = Nan::To<int>(info[2]).FromJust();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[3]));
  RegisterWorker *worker = new RegisterWorker(baton, callback, cleanupHandles);

  worker->Reference("filter_name", info[0]);
  worker->Reference("filter_priority", info[2]);

  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitFilterRegistry::RegisterWorker::AcquireLocks() {
  return nodegit::LockMaster(true, baton->filter_name, baton->filter);
}

void GitFilterRegistry::RegisterWorker::Execute() {
  git_error_clear();

  {
    int result = git_filter_register(baton->filter_name, baton->filter, baton->filter_priority);
    baton->error_code = result;

    if (result != GIT_OK && git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
}

void GitFilterRegistry::RegisterWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  free(baton->filter_name);

  delete baton;
}

void GitFilterRegistry::RegisterWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    static_pointer_cast<nodegit::FilterRegistryCleanupHandles>(nodegit::Context::GetCurrentContext()->GetCleanupHandle("filterRegistry"))->registeredFilters[baton->filter_name] = cleanupHandles["filter"];
    v8::Local<v8::Value> result = Nan::New(baton->error_code);
    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else if (baton->error) {
    v8::Local<v8::Object> err;
    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method register has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("FilterRegistry.register").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
      free((void *)baton->error->message);
    free((void *)baton->error);
  }
  else if (baton->error_code < 0) {
    v8::Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method register has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("FilterRegistry.register").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else {
    callback->Call(0, NULL, async_resource);
  }

  free(baton->filter_name);

  delete baton;
}

NAN_METHOD(GitFilterRegistry::GitFilterUnregister) {
  Nan::EscapableHandleScope scope;

  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("String name is required.");
  }

  if (info.Length() == 1 || !info[1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  FilterUnregisterBaton *baton = new FilterUnregisterBaton();
  Nan::Utf8String name(Nan::To<v8::String>(info[0]).ToLocalChecked());

  baton->filter_name = (char *)malloc(name.length() + 1);
  memcpy((void *)baton->filter_name, *name, name.length());
  memset((void *)(((char *)baton->filter_name) + name.length()), 0, 1);

  baton->error_code = GIT_OK;

  /* Setting up Async Worker */
  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));
  UnregisterWorker *worker = new UnregisterWorker(baton, callback);

  nodegit::Context *nodegitContext = reinterpret_cast<nodegit::Context *>(info.Data().As<External>()->Value());
  nodegitContext->QueueWorker(worker);
  return;
}

nodegit::LockMaster GitFilterRegistry::UnregisterWorker::AcquireLocks() {
  return nodegit::LockMaster(true, baton->filter_name);
}

void GitFilterRegistry::UnregisterWorker::Execute() {
  git_error_clear();

  {
    int result = git_filter_unregister(baton->filter_name);
    baton->error_code = result;

    if (result != GIT_OK && git_error_last() != NULL) {
      baton->error = git_error_dup(git_error_last());
    }
  }
}

void GitFilterRegistry::UnregisterWorker::HandleErrorCallback() {
  if (baton->error) {
    if (baton->error->message) {
      free((void *)baton->error->message);
    }

    free((void *)baton->error);
  }

  free(baton->filter_name);

  delete baton;
}

void GitFilterRegistry::UnregisterWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    static_pointer_cast<nodegit::FilterRegistryCleanupHandles>(nodegit::Context::GetCurrentContext()->GetCleanupHandle("filterRegistry"))->registeredFilters.erase(baton->filter_name);
    v8::Local<v8::Value> result = Nan::New(baton->error_code);
    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv, async_resource);
  }
  else if (baton->error) {
    v8::Local<v8::Object> err;
    if (baton->error->message) {
      err = Nan::To<v8::Object>(Nan::Error(baton->error->message)).ToLocalChecked();
    } else {
      err = Nan::To<v8::Object>(Nan::Error("Method register has thrown an error.")).ToLocalChecked();
    }
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("FilterRegistry.unregister").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
    if (baton->error->message)
      free((void *)baton->error->message);
    free((void *)baton->error);
  }
  else if (baton->error_code < 0) {
    v8::Local<v8::Object> err = Nan::To<v8::Object>(Nan::Error("Method unregister has thrown an error.")).ToLocalChecked();
    Nan::Set(err, Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
    Nan::Set(err, Nan::New("errorFunction").ToLocalChecked(), Nan::New("FilterRegistry.unregister").ToLocalChecked());
    v8::Local<v8::Value> argv[1] = {
      err
    };
    callback->Call(1, argv, async_resource);
  }
  else {
    callback->Call(0, NULL, async_resource);
  }

  free(baton->filter_name);

  delete baton;
}
