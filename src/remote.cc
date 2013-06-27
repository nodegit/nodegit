/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/remote.h"

#include "../include/functions/string.h"

using namespace v8;
using namespace node;

Remote::Remote(git_remote *raw) {
  this->raw = raw;
}

Remote::~Remote() {
  git_remote_free(this->raw);
}

void Remote::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Remote"));

  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_METHOD(tpl, "createInmemory", CreateInmemory);
  NODE_SET_METHOD(tpl, "load", Load);
  NODE_SET_PROTOTYPE_METHOD(tpl, "save", Save);
  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "url", Url);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushurl", Pushurl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUrl", SetUrl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPushurl", SetPushurl);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setFetchspec", SetFetchspec);
  NODE_SET_PROTOTYPE_METHOD(tpl, "fetchspec", Fetchspec);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setPushspec", SetPushspec);
  NODE_SET_PROTOTYPE_METHOD(tpl, "pushspec", Pushspec);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connect", Connect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "ls", Ls);
  NODE_SET_PROTOTYPE_METHOD(tpl, "download", Download);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connected", Connected);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);
  NODE_SET_PROTOTYPE_METHOD(tpl, "disconnect", Disconnect);
  NODE_SET_METHOD(tpl, "free", Free);
  NODE_SET_PROTOTYPE_METHOD(tpl, "updateTips", UpdateTips);
  NODE_SET_METHOD(tpl, "validUrl", ValidUrl);
  NODE_SET_METHOD(tpl, "supportedUrl", SupportedUrl);
  NODE_SET_METHOD(tpl, "list", List);
  NODE_SET_PROTOTYPE_METHOD(tpl, "checkCert", CheckCert);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setCredAcquireCb", SetCredAcquireCb);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setTransport", SetTransport);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setCallbacks", SetCallbacks);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stats", Stats);
  NODE_SET_METHOD(tpl, "autotag", Autotag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setAutotag", SetAutotag);
  NODE_SET_PROTOTYPE_METHOD(tpl, "rename", Rename);
  NODE_SET_PROTOTYPE_METHOD(tpl, "updateFetchhead", UpdateFetchhead);
  NODE_SET_PROTOTYPE_METHOD(tpl, "setUpdateFetchhead", SetUpdateFetchhead);
  NODE_SET_METHOD(tpl, "isValidName", IsValidName);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Remote"), constructor_template);
}

Handle<Value> Remote::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_remote is required.")));
  }

  Remote* object = new Remote((git_remote *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> Remote::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(Remote::constructor_template->NewInstance(1, argv));
}

git_remote *Remote::GetValue() {
  return this->raw;
}


Handle<Value> Remote::Create(const Arguments& args) {
  HandleScope scope;
  
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  CreateBaton* baton = new CreateBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->repoReference = Persistent<Value>::New(args[0]);
  baton->repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
  baton->nameReference = Persistent<Value>::New(args[1]);
  String::Utf8Value name(args[1]->ToString());
  baton->name = strdup(*name);
  baton->urlReference = Persistent<Value>::New(args[2]);
  String::Utf8Value url(args[2]->ToString());
  baton->url = strdup(*url);
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, CreateWork, (uv_after_work_cb)CreateAfterWork);

  return Undefined();
}

void Remote::CreateWork(uv_work_t *req) {
  CreateBaton *baton = static_cast<CreateBaton *>(req->data);
  int result = git_remote_create(
    &baton->out, 
    baton->repo, 
    baton->name, 
    baton->url
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void Remote::CreateAfterWork(uv_work_t *req) {
  HandleScope scope;
  CreateBaton *baton = static_cast<CreateBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = Remote::New((void *)baton->out);
  Handle<Value> result = to;
    Handle<Value> argv[2] = {
      Local<Value>::New(Null()),
      result
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  } else if (baton->error) {
    Handle<Value> argv[1] = {
      Exception::Error(String::New(baton->error->message))
    };
    baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  } else {
    baton->callback->Call(Context::GetCurrent()->Global(), 0, NULL);
  }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
  baton->repoReference.Dispose();
  baton->nameReference.Dispose();
  baton->urlReference.Dispose();
  baton->callback.Dispose();
  delete baton->name;
  delete baton->url;
  delete baton;
}

Handle<Value> Remote::CreateInmemory(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String fetch is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  git_remote *out = NULL;

  int result = git_remote_create_inmemory(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
    , stringArgToString(args[2]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = Remote::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Remote::Load(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String name is required.")));
  }

  git_remote *out = NULL;

  int result = git_remote_load(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
    , stringArgToString(args[1]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = Remote::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Remote::Save(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_save(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_name(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> Remote::Url(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_url(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> Remote::Pushurl(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_pushurl(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> Remote::SetUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("const url is required.")));
  }


  int result = git_remote_set_url(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<const>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::SetPushurl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("const url is required.")));
  }


  int result = git_remote_set_pushurl(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<const>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::SetFetchspec(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String spec is required.")));
  }


  int result = git_remote_set_fetchspec(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Fetchspec(const Arguments& args) {
  HandleScope scope;
  

  const git_refspec * result = git_remote_fetchspec(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Refspec::New((void *)result);
  return scope.Close(to);
}

Handle<Value> Remote::SetPushspec(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String spec is required.")));
  }


  int result = git_remote_set_pushspec(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Pushspec(const Arguments& args) {
  HandleScope scope;
  

  const git_refspec * result = git_remote_pushspec(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = Refspec::New((void *)result);
  return scope.Close(to);
}

Handle<Value> Remote::Connect(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Direction direction is required.")));
  }


  int result = git_remote_connect(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<Direction>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Ls(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("HeadlistCb list_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }


  int result = git_remote_ls(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<HeadlistCb>(args[0]->ToObject())->GetValue()
    , ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Download(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("TransferProgressCallback progress_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }


  int result = git_remote_download(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<TransferProgressCallback>(args[0]->ToObject())->GetValue()
    , ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Connected(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_connected(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Stop(const Arguments& args) {
  HandleScope scope;
  

  git_remote_stop(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  return Undefined();
}

Handle<Value> Remote::Disconnect(const Arguments& args) {
  HandleScope scope;
  

  git_remote_disconnect(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  return Undefined();
}

Handle<Value> Remote::Free(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Remote remote is required.")));
  }


  git_remote_free(
    ObjectWrap::Unwrap<Remote>(args[0]->ToObject())->GetValue()
  );


  return Undefined();
}

Handle<Value> Remote::UpdateTips(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_tips(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::ValidUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }


  int result = git_remote_valid_url(
    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::SupportedUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("const url is required.")));
  }


  int result = git_remote_supported_url(
    ObjectWrap::Unwrap<const>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::List(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  git_strarray out = NULL;

  int result = git_remote_list(
    &out
    , ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = Strarray::New((void *)out);
  return scope.Close(to);
}

Handle<Value> Remote::CheckCert(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number check is required.")));
  }


  git_remote_check_cert(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , (int) args[0]->ToInt32()->Value()
  );


  return Undefined();
}

Handle<Value> Remote::SetCredAcquireCb(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("CredAcquireCb cred_acquire_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }


  git_remote_set_cred_acquire_cb(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<CredAcquireCb>(args[0]->ToObject())->GetValue()
    , ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue()
  );


  return Undefined();
}

Handle<Value> Remote::SetTransport(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Transport transport is required.")));
  }


  int result = git_remote_set_transport(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<Transport>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::SetCallbacks(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("RemoteCallbacks callbacks is required.")));
  }


  int result = git_remote_set_callbacks(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<RemoteCallbacks>(args[0]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::Stats(const Arguments& args) {
  HandleScope scope;
  

  const git_transfer_progress * result = git_remote_stats(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );


  Handle<Value> to;
    to = TransferProgress::New((void *)result);
  return scope.Close(to);
}

Handle<Value> Remote::Autotag(const Arguments& args) {
  HandleScope scope;
  

  GIT_EXTERN( result = git_remote_autotag(
  );


  Handle<Value> to;
    to = GIT_EXTERN(::New((void *)result);
  return scope.Close(to);
}

Handle<Value> Remote::SetAutotag(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("RemoteAutotagOptionT value is required.")));
  }


  git_remote_set_autotag(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , ObjectWrap::Unwrap<RemoteAutotagOptionT>(args[0]->ToObject())->GetValue()
  );


  return Undefined();
}

Handle<Value> Remote::Rename(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String new_name is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("RemoteRenameProblemCb callback is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }


  int result = git_remote_rename(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , stringArgToString(args[0]->ToString()).c_str()
    , ObjectWrap::Unwrap<RemoteRenameProblemCb>(args[1]->ToObject())->GetValue()
    , ObjectWrap::Unwrap<void>(args[2]->ToObject())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::UpdateFetchhead(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_fetchhead(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> Remote::SetUpdateFetchhead(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number value is required.")));
  }


  git_remote_set_update_fetchhead(
    ObjectWrap::Unwrap<Remote>(args.This())->GetValue()
    , (int) args[0]->ToInt32()->Value()
  );


  return Undefined();
}

Handle<Value> Remote::IsValidName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String remote_name is required.")));
  }


  int result = git_remote_is_valid_name(
    stringArgToString(args[0]->ToString()).c_str()
  );

  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Persistent<Function> Remote::constructor_template;
