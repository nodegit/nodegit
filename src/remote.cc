/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Remote"));

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


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Remote"), _constructor_template);
}

NAN_METHOD(GitRemote::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_remote is required.");
  }
  GitRemote* object = new GitRemote(static_cast<git_remote *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitRemote::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitRemote::constructor_template)->NewInstance(1, argv));
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
    to = NanNew<String>(result);
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
    to = NanNew<String>(result);
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
    to = NanNew<String>(result);
  NanReturnValue(to);
}

/**
 * @param {String} url
 */
NAN_METHOD(GitRemote::SetUrl) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String url is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("String url is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("Number direction is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ConnectBaton* baton = new ConnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
    git_direction from_direction;
            from_direction = (git_direction) (int)  args[0]->ToNumber()->Value();
          baton->direction = from_direction;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  ConnectWorker *worker = new ConnectWorker(baton, callback);
  worker->SaveToPersistent("remote", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("direction", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRemote::ConnectWorker::Execute() {
  int result = git_remote_connect(
    baton->remote, 
    baton->direction
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitRemote::ConnectWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
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
    return NanThrowError("Callback is required and must be a Function.");
  }

  DownloadBaton* baton = new DownloadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();
    git_transfer_progress_callback from_progress_cb;
      if (args[0]->IsFunction()) {
                } else {
      from_progress_cb = 0;
    }
      baton->progress_cb = from_progress_cb;
      
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  DownloadWorker *worker = new DownloadWorker(baton, callback);
  worker->SaveToPersistent("remote", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("progress_cb", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("payload", args[1]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRemote::DownloadWorker::Execute() {
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

void GitRemote::DownloadWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("Callback is required and must be a Function.");
  }

  DisconnectBaton* baton = new DisconnectBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->remote = ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue();

  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[0]));
  DisconnectWorker *worker = new DisconnectWorker(baton, callback);
  worker->SaveToPersistent("remote", args.This());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitRemote::DisconnectWorker::Execute() {
  git_remote_disconnect(
    baton->remote
  );
}

void GitRemote::DisconnectWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
    Handle<Value> result = NanUndefined();
    Handle<Value> argv[2] = {
      NanNull(),
      result
    };
    callback->Call(2, argv);
  } else {
    if (baton->error) {
      Handle<Value> argv[1] = {
        NanError(baton->error->message)
      };
      callback->Call(1, argv);
      if (baton->error->message)
        free((void *)baton->error->message);
      free((void *)baton->error);
    } else {
      callback->Call(0, NULL);
    }
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("String url is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("String url is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("Number check is required.");
  }

  int from_check;
            from_check = (int)   args[0]->ToInt32()->Value();
      
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("Number value is required.");
  }

  int from_value;
            from_value = (int)   args[0]->ToInt32()->Value();
      
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
    return NanThrowError("String remote_name is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  NanReturnUndefined();
}

Persistent<Function> GitRemote::constructor_template;
