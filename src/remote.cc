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
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Remote"));

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


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Remote"), constructor_template);
}

Handle<Value> GitRemote::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_remote is required.")));
  }

  GitRemote* object = new GitRemote((git_remote *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitRemote::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitRemote::constructor_template->NewInstance(1, argv));
}

git_remote *GitRemote::GetValue() {
  return this->raw;
}


/**
 * @return {String} result
 */
Handle<Value> GitRemote::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_name(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitRemote::Url(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_url(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @return {String} result
 */
Handle<Value> GitRemote::PushUrl(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_pushurl(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

/**
 * @param {String} url
 */
Handle<Value> GitRemote::SetUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @param {String} url
 */
Handle<Value> GitRemote::SetPushUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

#include "../include/functions/copy.h"

/**
 * @param {Number} direction
 */
Handle<Value> GitRemote::Connect(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Number direction is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ConnectBaton* baton = new ConnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->remoteReference = Persistent<Value>::New(args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  baton->directionReference = Persistent<Value>::New(args[0]);
    git_direction from_direction;
            from_direction = (git_direction) (int)  args[0]->ToNumber()->Value();
          baton->direction = from_direction;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ConnectWork, (uv_after_work_cb)ConnectAfterWork);

  return Undefined();
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
  HandleScope scope;
  ConnectBaton *baton = static_cast<ConnectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitRemote::Download(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DownloadBaton* baton = new DownloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->remoteReference = Persistent<Value>::New(args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  baton->progress_cbReference = Persistent<Value>::New(args[0]);
    git_transfer_progress_callback from_progress_cb;
      if (args[0]->IsFunction()) {
            Persistent<Function>::New(Local<Function>::Cast(args[0]));
          } else {
      from_progress_cb = 0;
    }
      baton->progress_cb = from_progress_cb;
    baton->payloadReference = Persistent<Value>::New(args[1]);
      baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, DownloadWork, (uv_after_work_cb)DownloadAfterWork);

  return Undefined();
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
  HandleScope scope;
  DownloadBaton *baton = static_cast<DownloadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitRemote::Connected(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_connected(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
Handle<Value> GitRemote::Stop(const Arguments& args) {
  HandleScope scope;
  

  git_remote_stop(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  return Undefined();
}

#include "../include/functions/copy.h"

/**
 */
Handle<Value> GitRemote::Disconnect(const Arguments& args) {
  HandleScope scope;
    
  if (args.Length() == 0 || !args[0]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  DisconnectBaton* baton = new DisconnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->remoteReference = Persistent<Value>::New(args.This());
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));

  uv_queue_work(uv_default_loop(), &baton->request, DisconnectWork, (uv_after_work_cb)DisconnectAfterWork);

  return Undefined();
}

void GitRemote::DisconnectWork(uv_work_t *req) {
  DisconnectBaton *baton = static_cast<DisconnectBaton *>(req->data);
  git_remote_disconnect(
    baton->remote
  );
}

void GitRemote::DisconnectAfterWork(uv_work_t *req) {
  HandleScope scope;
  DisconnectBaton *baton = static_cast<DisconnectBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = Local<Value>::New(Undefined());
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        Exception::Error(String::New(baton->error->message))
      };
      baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
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
Handle<Value> GitRemote::UpdateTips(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_tips(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
 * @param {String} url
 */
Handle<Value> GitRemote::ValidUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @param {String} url
 */
Handle<Value> GitRemote::SupportedUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

/**
 * @param {Number} check
 */
Handle<Value> GitRemote::CheckCert(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number check is required.")));
  }

  int from_check;
            from_check = (int)   args[0]->ToInt32()->Value();
      
  git_remote_check_cert(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_check
  );

  return Undefined();
}

/**
 */
Handle<Value> GitRemote::UpdateFetchhead(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
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
 * @param {Number} value
 */
Handle<Value> GitRemote::SetUpdateFetchhead(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number value is required.")));
  }

  int from_value;
            from_value = (int)   args[0]->ToInt32()->Value();
      
  git_remote_set_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_value
  );

  return Undefined();
}

/**
 * @param {String} remote_name
 */
Handle<Value> GitRemote::IsValidName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String remote_name is required.")));
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
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

Persistent<Function> GitRemote::constructor_template;
