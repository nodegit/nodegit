#include "../../include/functions/utilities.h"

using namespace v8;

bool success(const git_error* error, Persistent<Function> callback) {
  HandleScope scope;

  if (error) {
    Local<Value> argv[1] = {
      GitError::WrapError(error)
    };

    TryCatch try_catch;
    callback->Call(Context::GetCurrent()->Global(), 1, argv);
    if (try_catch.HasCaught()) {
        FatalException(try_catch);
    }
    return false;
  }
  return true;
}
