/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/threads.h"

using namespace v8;
using namespace node;

void GitThreads::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<Object> object = NanNew<Object>();

  NODE_SET_METHOD(object, "init", Init);
  NODE_SET_METHOD(object, "shutdown", Shutdown);

  target->Set(NanNew<String>("Threads"), object);
}


/**
 */
NAN_METHOD(GitThreads::Init) {
  NanScope();
  

  int result = git_threads_init(
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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

