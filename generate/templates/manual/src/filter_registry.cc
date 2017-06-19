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
  // end convert_from_v8 block
  // start convert_from_v8 block
  git_filter * from_filter = NULL;
  from_filter = Nan::ObjectWrap::Unwrap<GitFilter>(info[1]->ToObject())->GetValue();
  // end convert_from_v8 block
  // start convert_from_v8 block
  int from_priority;
  from_priority = (int) info[2]->ToNumber()->Value();
  // end convert_from_v8 block
  bool result = GitFilterRegistry::persistentHandle.IsEmpty();
  Nan::New(GitFilterRegistry::persistentHandle)->Set(info[0]->ToString(), info[1]->ToObject());
  // Nan::Persistent<v8::Object> testSample = GitFilterRegistry::persistentHandle;
  // Nan::New<v8::Object>(GitFilterRegistry::persistentHandle)->Set(name, info.This());
  // Nan::Set(GitFilterRegistry::persistentHandle, name, info.This());
  v8::Local<Object> temp = Nan::New<v8::Object>(GitFilterRegistry::persistentHandle);
  // v8::Local<v8::String> key = Nan::New<String>("omg").ToLocalChecked();
  v8::Local<v8::String> key = info[0]->ToString();
  v8::Maybe<bool> result2 = Nan::Has(temp, key);
  // New(GitFilterRegistry::persistentHandle)->Set(name, in)
  // v8::New(persistentHandle).Set(name, info.This());
  // v8::Local<v8::Object> testObject = Nan::New(GitFilterRegistry::persistentHandle);
  // testObject->Set(name, from_filter);
  // Nan::Set(testObject, Nan::New<String>("omg").ToLocalChecked(), info[1]->ToObject());

  // v8::Local<v8::Object> testObject2 = Nan::New(GitFilterRegistry::persistentHandle);
  // Nan::MaybeLocal<Value> res1 = Nan::Get(testObject2, Nan::New<String>("omg").ToLocalChecked());

  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */false, from_name, from_filter);

    int result = git_filter_register(from_name, from_filter, from_priority);
 
    v8::Local<v8::Value> to;
    // start convert_to_v8 block
    to = Nan::New<Number>(result);
    // end convert_to_v8 block
    return info.GetReturnValue().Set(scope.Escape(to));
  }
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
  // end convert_from_v8 block
 
  giterr_clear();

  {
    LockMaster lockMaster(/*asyncAction: */false, from_name);

    int result = git_filter_unregister(from_name);
 
    v8::Local<v8::Value> to;
    // start convert_to_v8 block
    to = Nan::New<Number>(result);
    // end convert_to_v8 block
    return info.GetReturnValue().Set(scope.Escape(to));
  }
}
