/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/odb.h"
#include "../include/oid.h"
#include "../include/odb_object.h"
#include "node_buffer.h"

using namespace v8;
using namespace node;

GitOdb::GitOdb(git_odb *raw) {
  this->raw = raw;
}

GitOdb::~GitOdb() {
  git_odb_free(this->raw);
}

void GitOdb::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Odb"));

  NODE_SET_METHOD(tpl, "create()", Create);
  NODE_SET_METHOD(tpl, "open", Open);
  NODE_SET_PROTOTYPE_METHOD(tpl, "addDiskAlternate", AddDiskAlternate);
  NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
  NODE_SET_METHOD(tpl, "readPrefix", ReadPrefix);
  NODE_SET_METHOD(tpl, "readHeader", ReadHeader);
  NODE_SET_PROTOTYPE_METHOD(tpl, "exists", Exists);
  NODE_SET_PROTOTYPE_METHOD(tpl, "refresh", Refresh);
  NODE_SET_METHOD(tpl, "write", Write);
  NODE_SET_METHOD(tpl, "hash", Hash);
  NODE_SET_METHOD(tpl, "hashfile", Hashfile);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Odb"), constructor_template);
}

Handle<Value> GitOdb::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_odb is required.")));
  }

  GitOdb* object = new GitOdb((git_odb *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitOdb::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitOdb::constructor_template->NewInstance(1, argv));
}

git_odb *GitOdb::GetValue() {
  return this->raw;
}


Handle<Value> GitOdb::Create(const Arguments& args) {
  HandleScope scope;
  
  git_odb *out = NULL;

  int result = git_odb_new(
    &out
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdb::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::Open(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String objects_dir is required.")));
  }

  git_odb *out = NULL;
  String::Utf8Value objects_dir(args[0]->ToString());
  const char * from_objects_dir = strdup(*objects_dir);

  int result = git_odb_open(
    &out
    , from_objects_dir
  );
  delete from_objects_dir;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdb::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::AddDiskAlternate(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }

  String::Utf8Value path(args[0]->ToString());
  const char * from_path = strdup(*path);

  int result = git_odb_add_disk_alternate(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_path
  );
  delete from_path;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitOdb::Read(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  git_odb_object *out = NULL;
  const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();

  int result = git_odb_read(
    &out
    , ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_id
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdbObject::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::ReadPrefix(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Odb db is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid short_id is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }

  git_odb_object *out = NULL;
  git_odb * from_db = ObjectWrap::Unwrap<GitOdb>(args[0]->ToObject())->GetValue();
  const git_oid * from_short_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
  size_t from_len = (size_t) args[2]->ToUint32()->Value();

  int result = git_odb_read_prefix(
    &out
    , from_db
    , from_short_id
    , from_len
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdbObject::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::ReadHeader(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number len_out is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type_out is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Odb db is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  size_t * from_len_out = (size_t *) args[0]->ToUint32()->Value();
  git_otype * from_type_out = (git_otype *) args[1]->ToInt32()->Value();
  git_odb * from_db = ObjectWrap::Unwrap<GitOdb>(args[2]->ToObject())->GetValue();
  const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[3]->ToObject())->GetValue();

  int result = git_odb_read_header(
    from_len_out
    , from_type_out
    , from_db
    , from_id
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitOdb::Exists(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();

  int result = git_odb_exists(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_id
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitOdb::Refresh(const Arguments& args) {
  HandleScope scope;
  

  int result = git_odb_refresh(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

Handle<Value> GitOdb::Write(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Buffer data is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid ));
  const void * from_data = Buffer::Data(ObjectWrap::Unwrap<Buffer>(args[0]->ToObject()));
  size_t from_len = (size_t) args[1]->ToUint32()->Value();
  git_otype from_type = (git_otype) args[2]->ToInt32()->Value();

  int result = git_odb_write(
    out
    , ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_data
    , from_len
    , from_type
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOid::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::Hash(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Buffer data is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid ));
  const void * from_data = Buffer::Data(ObjectWrap::Unwrap<Buffer>(args[0]->ToObject()));
  size_t from_len = (size_t) args[1]->ToUint32()->Value();
  git_otype from_type = (git_otype) args[2]->ToInt32()->Value();

  int result = git_odb_hash(
    out
    , from_data
    , from_len
    , from_type
  );
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOid::New((void *)out);
  return scope.Close(to);
}

Handle<Value> GitOdb::Hashfile(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String path is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid ));
  String::Utf8Value path(args[0]->ToString());
  const char * from_path = strdup(*path);
  git_otype from_type = (git_otype) args[1]->ToInt32()->Value();

  int result = git_odb_hashfile(
    out
    , from_path
    , from_type
  );
  delete from_path;
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOid::New((void *)out);
  return scope.Close(to);
}

Persistent<Function> GitOdb::constructor_template;
