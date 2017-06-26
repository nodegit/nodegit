// This is a generated file, modify: generate/templates/templates/class_content.cc

#include <nan.h>
#include <string.h>

extern "C" {
  #include <git2.h>
 }

#include "../include/nodegit.h"
#include "../include/lock_master.h"
#include "../include/functions/copy.h"
#include "../include/filter_registry.h"
#include "nodegit_wrapper.cc"
#include "../include/async_libgit2_queue_worker.h"

#include "../include/filter.h"
 
#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

Nan::Persistent<v8::Object> GitFilterRegistry::persistentHandle;

// #pragma unmanaged
void GitFilterRegistry::InitializeComponent(v8::Local<v8::Object> target) {
  Nan::HandleScope scope;


  v8::Local<Object> object = Nan::New<Object>();

  Nan::SetMethod(object, "register", GitFilterRegister);
  Nan::SetMethod(object, "unregister", GitFilterUnregister);

  
  Nan::Set(target, Nan::New<String>("FilterRegistry").ToLocalChecked(), object);
  GitFilterRegistry::persistentHandle.Reset(object);
}
// TODO: Reset persistent handle in destructor
/*
 * @param String name
* @param Filter filter
* @param Number priority
* @return Number  result
*/

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
  // start convert_from_v8 block
  const char * from_name = NULL;

  String::Utf8Value name(info[0]->ToString());
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_name = (const char *) malloc(name.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_name, *name, name.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_name) + name.length()), 0, 1);

  FilterBaton *baton = new FilterBaton;
  
  git_filter *from_filter = NULL;
  from_filter = Nan::ObjectWrap::Unwrap<GitFilter>(info[1]->ToObject())->GetValue();
  
  baton->filter = from_filter;
  baton->filter_name = (char *) malloc(name.length() + 1);
  strcpy(baton->filter_name, from_name);
  baton->error_code = GIT_OK;

  int from_priority;
  from_priority = (int) info[2]->ToNumber()->Value();

  baton->filter_priority = from_priority;

  /* This will delete the filter name from persistent handle */
  bool result = GitFilterRegistry::persistentHandle.IsEmpty();
  Nan::New(GitFilterRegistry::persistentHandle)->Set(info[0]->ToString(), info[1]->ToObject());

  v8::Local<Object> handleRef = Nan::New<v8::Object>(GitFilterRegistry::persistentHandle);
  v8::Local<v8::String> key = info[0]->ToString();
  v8::Maybe<bool> result2 = Nan::Has(handleRef, key);

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[3]));
  RegisterWorker *worker = new RegisterWorker(baton, callback);

  worker->SaveToPersistent("filter_name", info[0]->ToObject());
  worker->SaveToPersistent("filter_priority", info[2]->ToObject());

  AsyncLibgit2QueueWorker(worker);
  return;
}
// no v8 in execute
void GitFilterRegistry::RegisterWorker::Execute() {
  
  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */true, baton->filter_name, baton->filter);
    int result = git_filter_register(baton->filter_name, baton->filter, baton->filter_priority);
    baton->error_code = result;

    if (result != GIT_OK && giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
  }
}

void GitFilterRegistry::RegisterWorker::HandleOKCallback() {
  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> result = Nan::New(baton->error_code);
    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      v8::Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::Error(baton->error->message)->ToObject();
      } else {
        err = Nan::Error("Method register has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      v8::Local<v8::Value> argv[1] = {
        err
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      std::queue< v8::Local<v8::Value> > workerArguments;
      workerArguments.push(GetFromPersistent("filter_name"));
      workerArguments.push(GetFromPersistent("filter_priority"));

      bool callbackFired = false;
      while(!workerArguments.empty()) {
        v8::Local<v8::Value> node = workerArguments.front();
        workerArguments.pop();

        if (
          !node->IsObject()
          || node->IsArray()
          || node->IsBooleanObject()
          || node->IsDate()
          || node->IsFunction()
          || node->IsNumberObject()
          || node->IsRegExp()
          || node->IsStringObject()
        ) {
          continue;
        }

        v8::Local<v8::Object> nodeObj = node->ToObject();
        v8::Local<v8::Value> checkValue = GetPrivate(nodeObj, Nan::New("NodeGitPromiseError").ToLocalChecked());

        if (!checkValue.IsEmpty() && !checkValue->IsNull() && !checkValue->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {
            checkValue->ToObject()
          };
          callback->Call(1, argv);
          callbackFired = true;
          break;
        }

        v8::Local<v8::Array> properties = nodeObj->GetPropertyNames();
        for (unsigned int propIndex = 0; propIndex < properties->Length(); ++propIndex) {
          v8::Local<v8::String> propName = properties->Get(propIndex)->ToString();
          v8::Local<v8::Value> nodeToQueue = nodeObj->Get(propName);
          if (!nodeToQueue->IsUndefined()) {
            workerArguments.push(nodeToQueue);
          }
        }
      }

      if (!callbackFired) {
        v8::Local<v8::Object> err = Nan::Error("Method register has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        v8::Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv);
      }
    } else {
      callback->Call(0, NULL);
    }
  }
  delete baton;
  return;
}
   
/*
* @param String name
* @return Number  result    
*/

NAN_METHOD(GitFilterRegistry::GitFilterUnregister) {
  Nan::EscapableHandleScope scope;

  if (info.Length() == 0 || !info[0]->IsString()) {
    return Nan::ThrowError("String name is required.");
  }

  if (info.Length() == 1 || !info[1]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  const char * from_name = NULL;

  String::Utf8Value name(info[0]->ToString());
  // malloc with one extra byte so we can add the terminating null character C-strings expect:
  from_name = (const char *) malloc(name.length() + 1);
  // copy the characters from the nodejs string into our C-string (used instead of strdup or strcpy because nulls in
  // the middle of strings are valid coming from nodejs):
  memcpy((void *)from_name, *name, name.length());
  // ensure the final byte of our new string is null, extra casts added to ensure compatibility with various C types
  // used in the nodejs binding generation:
  memset((void *)(((char *)from_name) + name.length()), 0, 1);

  SimpleFilterBaton *baton = new SimpleFilterBaton;
  baton->filter_name = (char *) malloc(name.length() + 1);
  strcpy(baton->filter_name, from_name);
  baton->error_code = GIT_OK;

  /* Setting up Async Worker */
  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[1]));
  UnRegisterWorker *worker = new UnRegisterWorker(baton, callback);

  worker->SaveToPersistent("filter_name", info[0]);

  /*giterr_clear();

  {
    LockMaster lockMaster(false, from_name);

    int result = git_filter_unregister(from_name);
 
    v8::Local<v8::Value> to;
    // start convert_to_v8 block
    to = Nan::New<Number>(result);
    // end convert_to_v8 block
    return info.GetReturnValue().Set(scope.Escape(to));
  }*/
  // Remove persistent reference for given filter
  /*v8::Local<Object> handleRef = Nan::New<v8::Object>(GitFilterRegistry::persistentHandle);
  Nan::Maybe<bool> _delete_result = Nan::Delete(handleRef, info[0]->ToString());

  Nan::Maybe<bool> result3 = Nan::Has(handleRef, info[0]->ToString());

  if(!GitFilterRegistry::persistentHandle.IsEmpty()){
    printf("not empty\n");
    GitFilterRegistry::persistentHandle.Reset();
  } else {
    printf("empty\n");
  }*/

  AsyncLibgit2QueueWorker(worker);
  return;
}

// no v8 in execute
void GitFilterRegistry::UnRegisterWorker::Execute() {
  
  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */true, baton->filter_name);
    int result = git_filter_unregister(baton->filter_name);
    baton->error_code = result;

    if (result != GIT_OK && giterr_last() != NULL) {
      baton->error = git_error_dup(giterr_last());
    }
  }
  /*// Remove persistent reference for given filter
  *v8::Local<Object> handleRef = Nan::New<v8::Object>(GitFilterRegistry::persistentHandle);
  Nan::Maybe<bool> _delete_result = Nan::Delete(handleRef, info[0]->ToString());

  Nan::Maybe<bool> result3 = Nan::Has(handleRef, info[0]->ToString());

  if(!GitFilterRegistry::persistentHandle.IsEmpty()){
    printf("not empty\n");
    GitFilterRegistry::persistentHandle.Reset();
  } else {
    printf("empty\n");
  }*/
}

void GitFilterRegistry::UnRegisterWorker::HandleOKCallback() {

  if (baton->error_code == GIT_OK) {
    v8::Local<v8::Value> result = Nan::New(baton->error_code);
    v8::Local<v8::Value> argv[2] = {
      Nan::Null(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      v8::Local<v8::Object> err;
      if (baton->error->message) {
        err = Nan::Error(baton->error->message)->ToObject();
      } else {
        err = Nan::Error("Method register has thrown an error.")->ToObject();
      }
      err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
      v8::Local<v8::Value> argv[1] = {
        err
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else if (baton->error_code < 0) {
      std::queue< v8::Local<v8::Value> > workerArguments;
      workerArguments.push(GetFromPersistent("filter_name"));
      
      bool callbackFired = false;
      while(!workerArguments.empty()) {
        v8::Local<v8::Value> node = workerArguments.front();
        workerArguments.pop();

        if (
          !node->IsObject()
          || node->IsArray()
          || node->IsBooleanObject()
          || node->IsDate()
          || node->IsFunction()
          || node->IsNumberObject()
          || node->IsRegExp()
          || node->IsStringObject()
        ) {
          continue;
        }

        v8::Local<v8::Object> nodeObj = node->ToObject();
        v8::Local<v8::Value> checkValue = GetPrivate(nodeObj, Nan::New("NodeGitPromiseError").ToLocalChecked());

        if (!checkValue.IsEmpty() && !checkValue->IsNull() && !checkValue->IsUndefined()) {
          v8::Local<v8::Value> argv[1] = {
            checkValue->ToObject()
          };
          callback->Call(1, argv);
          callbackFired = true;
          break;
        }

        v8::Local<v8::Array> properties = nodeObj->GetPropertyNames();
        for (unsigned int propIndex = 0; propIndex < properties->Length(); ++propIndex) {
          v8::Local<v8::String> propName = properties->Get(propIndex)->ToString();
          v8::Local<v8::Value> nodeToQueue = nodeObj->Get(propName);
          if (!nodeToQueue->IsUndefined()) {
            workerArguments.push(nodeToQueue);
          }
        }
      }

      if (!callbackFired) {
        v8::Local<v8::Object> err = Nan::Error("Method register has thrown an error.")->ToObject();
        err->Set(Nan::New("errno").ToLocalChecked(), Nan::New(baton->error_code));
        v8::Local<v8::Value> argv[1] = {
          err
        };
        callback->Call(1, argv);
      }
    } else {
      callback->Call(0, NULL);
    }
  }
  delete baton;
  return;
}
