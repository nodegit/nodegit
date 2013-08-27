/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/threads.h"

using namespace v8;
using namespace node;

void GitThreads::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Persistent<Object> object = Persistent<Object>::New(Object::New());

  object->Set(String::NewSymbol("init"), FunctionTemplate::New(Init)->GetFunction());
  object->Set(String::NewSymbol("shutdown"), FunctionTemplate::New(Shutdown)->GetFunction());

  target->Set(String::NewSymbol("Threads"), object);
}


/**
 */
Handle<Value> GitThreads::Init(const Arguments& args) {
  HandleScope scope;
  

  int result = git_threads_init(
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 */
Handle<Value> GitThreads::Shutdown(const Arguments& args) {
  HandleScope scope;
  

  git_threads_shutdown(
  );

  return Undefined();
}

