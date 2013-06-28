/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/remote.h"

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


Handle<Value> GitRemote::Save(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_save(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Name(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_name(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitRemote::Url(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_url(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitRemote::Pushurl(const Arguments& args) {
  HandleScope scope;
  

  const char * result = git_remote_pushurl(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = String::New(result);
  return scope.Close(to);
}

Handle<Value> GitRemote::SetUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  String::Utf8Value url(args[0]->ToString());
  const char* from_url = strdup(*url);

  int result = git_remote_set_url(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_url
  );
  delete from_url;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::SetPushurl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  String::Utf8Value url(args[0]->ToString());
  const char* from_url = strdup(*url);

  int result = git_remote_set_pushurl(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_url
  );
  delete from_url;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::SetFetchspec(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String spec is required.")));
  }

  String::Utf8Value spec(args[0]->ToString());
  const char * from_spec = strdup(*spec);

  int result = git_remote_set_fetchspec(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_spec
  );
  delete from_spec;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Fetchspec(const Arguments& args) {
  HandleScope scope;
  

  const git_refspec * result = git_remote_fetchspec(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Refspec::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitRemote::SetPushspec(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String spec is required.")));
  }

  String::Utf8Value spec(args[0]->ToString());
  const char * from_spec = strdup(*spec);

  int result = git_remote_set_pushspec(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_spec
  );
  delete from_spec;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Pushspec(const Arguments& args) {
  HandleScope scope;
  

  const git_refspec * result = git_remote_pushspec(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = Refspec::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitRemote::Connect(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Direction direction is required.")));
  }

  git_direction from_direction = ObjectWrap::Unwrap<Direction>(args[0]->ToObject())->GetValue();

  int result = git_remote_connect(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_direction
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Ls(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("HeadlistCb list_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }

  git_headlist_cb from_list_cb = ObjectWrap::Unwrap<HeadlistCb>(args[0]->ToObject())->GetValue();
  void * from_payload = ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue();

  int result = git_remote_ls(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_list_cb
    , from_payload
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Download(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("TransferProgressCallback progress_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }

  git_transfer_progress_callback from_progress_cb = ObjectWrap::Unwrap<TransferProgressCallback>(args[0]->ToObject())->GetValue();
  void * from_payload = ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue();

  int result = git_remote_download(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_progress_cb
    , from_payload
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Connected(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_connected(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Stop(const Arguments& args) {
  HandleScope scope;
  

  git_remote_stop(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  return Undefined();
}

Handle<Value> GitRemote::Disconnect(const Arguments& args) {
  HandleScope scope;
  

  git_remote_disconnect(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  return Undefined();
}

Handle<Value> GitRemote::Free(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Remote remote is required.")));
  }

  git_remote * from_remote = ObjectWrap::Unwrap<GitRemote>(args[0]->ToObject())->GetValue();

  git_remote_free(
    from_remote
  );

  return Undefined();
}

Handle<Value> GitRemote::UpdateTips(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_tips(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::ValidUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  String::Utf8Value url(args[0]->ToString());
  const char * from_url = strdup(*url);

  int result = git_remote_valid_url(
    from_url
  );
  delete from_url;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::SupportedUrl(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String url is required.")));
  }

  String::Utf8Value url(args[0]->ToString());
  const char* from_url = strdup(*url);

  int result = git_remote_supported_url(
    from_url
  );
  delete from_url;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::List(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }

  git_strarray out = NULL;
  git_repository * from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();

  int result = git_remote_list(
    &out
    , from_repo
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = Strarray::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitRemote::CheckCert(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number check is required.")));
  }

  int from_check = (int) args[0]->ToInt32()->Value();

  git_remote_check_cert(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_check
  );

  return Undefined();
}

Handle<Value> GitRemote::SetCredAcquireCb(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("CredAcquireCb cred_acquire_cb is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }

  git_cred_acquire_cb from_cred_acquire_cb = ObjectWrap::Unwrap<CredAcquireCb>(args[0]->ToObject())->GetValue();
  void * from_payload = ObjectWrap::Unwrap<void>(args[1]->ToObject())->GetValue();

  git_remote_set_cred_acquire_cb(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_cred_acquire_cb
    , from_payload
  );

  return Undefined();
}

Handle<Value> GitRemote::SetTransport(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Transport transport is required.")));
  }

  git_transport * from_transport = ObjectWrap::Unwrap<Transport>(args[0]->ToObject())->GetValue();

  int result = git_remote_set_transport(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_transport
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::SetCallbacks(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("RemoteCallbacks callbacks is required.")));
  }

  git_remote_callbacks * from_callbacks = ObjectWrap::Unwrap<RemoteCallbacks>(args[0]->ToObject())->GetValue();

  int result = git_remote_set_callbacks(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_callbacks
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::Stats(const Arguments& args) {
  HandleScope scope;
  

  const git_transfer_progress * result = git_remote_stats(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );

  Handle<Value> to;
    to = TransferProgress::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitRemote::Autotag(const Arguments& args) {
  HandleScope scope;
  

  GIT_EXTERN( result = git_remote_autotag(
  );

  Handle<Value> to;
    to = GIT_EXTERN(::New((void *)result);
  return scope.Close(to);
}

Handle<Value> GitRemote::SetAutotag(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("RemoteAutotagOptionT value is required.")));
  }

  git_remote_autotag_option_t from_value = ObjectWrap::Unwrap<RemoteAutotagOptionT>(args[0]->ToObject())->GetValue();

  git_remote_set_autotag(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_value
  );

  return Undefined();
}

Handle<Value> GitRemote::Rename(const Arguments& args) {
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

  String::Utf8Value new_name(args[0]->ToString());
  const char * from_new_name = strdup(*new_name);
  git_remote_rename_problem_cb from_callback = ObjectWrap::Unwrap<RemoteRenameProblemCb>(args[1]->ToObject())->GetValue();
  void * from_payload = ObjectWrap::Unwrap<void>(args[2]->ToObject())->GetValue();

  int result = git_remote_rename(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_new_name
    , from_callback
    , from_payload
  );
  delete from_new_name;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::UpdateFetchhead(const Arguments& args) {
  HandleScope scope;
  

  int result = git_remote_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitRemote::SetUpdateFetchhead(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number value is required.")));
  }

  int from_value = (int) args[0]->ToInt32()->Value();

  git_remote_set_update_fetchhead(
    ObjectWrap::Unwrap<GitRemote>(args.This())->GetValue()
    , from_value
  );

  return Undefined();
}

Handle<Value> GitRemote::IsValidName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String remote_name is required.")));
  }

  String::Utf8Value remote_name(args[0]->ToString());
  const char * from_remote_name = strdup(*remote_name);

  int result = git_remote_is_valid_name(
    from_remote_name
  );
  delete from_remote_name;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Persistent<Function> GitRemote::constructor_template;
