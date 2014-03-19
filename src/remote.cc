/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/remote.h"
#include "git2/net.h"

using namespace v8;
using namespace node;

GitRemote::GitRemote(git_remote *raw) {
  this->raw = raw;
}

GitRemote::~GitRemote() {
  git_remote_free(this->raw);
}

void GitRemote::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Remote"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "url", Url);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushUrl", PushUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUrl", SetUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPushUrl", SetPushUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "download", Download);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connected", Connected);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "disconnect", Disconnect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "updateTips", UpdateTips);
  NODE_SET_METHOD(tpl, "validUrl", ValidUrl);
  NODE_SET_METHOD(tpl, "supportedUrl", SupportedUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "checkCert", CheckCert);
  NODE_SET_PROTOTYPE_METHOD(tpl, "updateFetchhead", UpdateFetchhead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUpdateFetchhead", SetUpdateFetchhead);
  NODE_SET_METHOD(tpl, "isValidName", IsValidName);


  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Remote"), tpl->GetFunction());
}

NAN_METHOD(GitRemote::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_remote is required."));
  }

  GitRemote* object = new GitRemote((git_remote *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRemote::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_remote *GitRemote::GetValue() {
  return this->raw;
}


/**
 * @return {String} result
 */
NAN_METHOD(GitRemote::Name) {
  NanScope();
  

  const char * result = git_remote_name(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitRemote::Url) {
  NanScope();
  

  const char * result = git_remote_url(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @return {String} result
 */
NAN_METHOD(GitRemote::PushUrl) {
  NanScope();
  

  const char * result = git_remote_pushurl(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  NanReturnValue(to);
}

/**
 * @param {String} url
 */
NAN_METHOD(GitRemote::SetUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String url is required."));
  }

  const char* from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
      
  int result = git_remote_set_url(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_url
  );
  free((void *)from_url);
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
 * @param {String} url
 */
NAN_METHOD(GitRemote::SetPushUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String url is required."));
  }

  const char* from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
      
  int result = git_remote_set_pushurl(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_url
  );
  free((void *)from_url);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

#include "../include/functions/copy.h"

/**
 * @param {Number} direction
 */
NAN_METHOD(GitRemote::Connect) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsNumber()) {
    return NanThrowError(String::New("Number direction is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ConnectBaton* baton = new ConnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->remoteReference, args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->directionReference, args[0]);
  git_direction from_direction;
  from_direction = (git_direction) args[0]->ToNumber()->Value();
  baton->direction = from_direction;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ConnectWork, (uv_after_work_cb)ConnectAfterWork);

  NanReturnUndefined();
}

void GitRemote::ConnectWork(uv_work_t *req) {
  ConnectBaton *baton = static_cast<ConnectBaton *>(req->data);
  int result = git_remote_connect(
    baton->remote, 
    baton->direction
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRemote::ConnectAfterWork(uv_work_t *req) {
  NanScope();
  ConnectBaton *baton = static_cast<ConnectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
      result
    };
    NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->remoteReference.Dispose();
  baton->directionReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

#include "../include/functions/copy.h"

/**
 * @param {Function} progress_cb
 * @param {void} payload
 */
NAN_METHOD(GitRemote::Download) {
  NanScope();
    
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  DownloadBaton* baton = new DownloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->remoteReference, args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->progress_cbReference, args[0]);
  git_transfer_progress_callback from_progress_cb;
  if (!args[0]->IsFunction()) {
    //TODO Make progress callback work
    //NanAssignPersistent(Function, baton->progress_cb, Local<Function>::Cast(args[0]));
  } else {
    from_progress_cb = 0;
  }
  baton->progress_cb = from_progress_cb;
  NanAssignPersistent(Value, baton->payloadReference, args[1]);
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, DownloadWork, (uv_after_work_cb)DownloadAfterWork);

  NanReturnUndefined();
}

void GitRemote::DownloadWork(uv_work_t *req) {
  DownloadBaton *baton = static_cast<DownloadBaton *>(req->data);
  int result = git_remote_download(
    baton->remote, 
    baton->progress_cb, 
    baton->payload
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRemote::DownloadAfterWork(uv_work_t *req) {
  NanScope();
  DownloadBaton *baton = static_cast<DownloadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
      result
    };
    NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->remoteReference.Dispose();
  baton->progress_cbReference.Dispose();
  baton->payloadReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 */
NAN_METHOD(GitRemote::Connected) {
  NanScope();
  

  int result = git_remote_connected(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
NAN_METHOD(GitRemote::Stop) {
  NanScope();
  

  git_remote_stop(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  NanReturnUndefined();
}

#include "../include/functions/copy.h"

/**
 */
NAN_METHOD(GitRemote::Disconnect) {
  NanScope();
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  DisconnectBaton* baton = new DisconnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->remoteReference, args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, DisconnectWork, (uv_after_work_cb)DisconnectAfterWork);

  NanReturnUndefined();
}

void GitRemote::DisconnectWork(uv_work_t *req) {
  DisconnectBaton *baton = static_cast<DisconnectBaton *>(req->data);
  git_remote_disconnect(
    baton->remote
  );
}

void GitRemote::DisconnectAfterWork(uv_work_t *req) {
  NanScope();
  DisconnectBaton *baton = static_cast<DisconnectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanNewLocal<Value>(Undefined());
    Handle<Value> argv[2] = {
      NanNewLocal<Value>(Null()),
      result
    };
    NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      NanPersistentToLocal(baton->callback)->Call(Context::GetCurrent()->Global(), 0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->remoteReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 */
NAN_METHOD(GitRemote::UpdateTips) {
  NanScope();
  

  int result = git_remote_update_tips(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
 * @param {String} url
 */
NAN_METHOD(GitRemote::ValidUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String url is required."));
  }

  const char * from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
      
  int result = git_remote_valid_url(
    from_url
  );
  free((void *)from_url);
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
 * @param {String} url
 */
NAN_METHOD(GitRemote::SupportedUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String url is required."));
  }

  const char* from_url;
            String::Utf8Value url(args[0]->ToString());
      from_url = strdup(*url);
      
  int result = git_remote_supported_url(
    from_url
  );
  free((void *)from_url);
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
 * @param {Number} check
 */
NAN_METHOD(GitRemote::CheckCert) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number check is required."));
  }

  int from_check;
            from_check = (int) args[0]->ToInt32()->Value();
      
  git_remote_check_cert(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_check
  );

  NanReturnUndefined();
}

/**
 */
NAN_METHOD(GitRemote::UpdateFetchhead) {
  NanScope();
  

  int result = git_remote_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
 * @param {Number} value
 */
NAN_METHOD(GitRemote::SetUpdateFetchhead) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return NanThrowError(String::New("Number value is required."));
  }

  int from_value;
            from_value = (int) args[0]->ToInt32()->Value();
      
  git_remote_set_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_value
  );

  NanReturnUndefined();
}

/**
 * @param {String} remote_name
 */
NAN_METHOD(GitRemote::IsValidName) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String remote_name is required."));
  }

  const char * from_remote_name;
            String::Utf8Value remote_name(args[0]->ToString());
      from_remote_name = strdup(*remote_name);
      
  int result = git_remote_is_valid_name(
    from_remote_name
  );
  free((void *)from_remote_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  NanReturnUndefined();
}

Persistent<FunctionTemplate> GitRemote::constructor_template;
