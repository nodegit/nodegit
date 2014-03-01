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
  NanScope();

  Persistent<Object> object;

  NanAssignPersistent(Object, object, Object::New());

  NanPersistentToLocal(object)->Set(String::NewSymbol("init"), FunctionTemplate::New(Init)->GetFunction());
  NanPersistentToLocal(object)->Set(String::NewSymbol("shutdown"), FunctionTemplate::New(Shutdown)->GetFunction());

  target->Set(String::NewSymbol("Threads"), NanPersistentToLocal(object));
}


/**
 */
NAN_METHOD(GitThreads::Init) {
  NanScope();
  

  int result = git_threads_init(
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

/**
 */
NAN_METHOD(GitThreads::Shutdown) {
  NanScope();
  

  git_threads_shutdown(
  );

  NanReturnUndefined();
}

