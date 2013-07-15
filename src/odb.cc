/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

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
  NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
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


/**
 * @return {Odb} out
 */
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

/**
 * @param {String} objects_dir
 * @return {Odb} out
 */
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
  free((void *)from_objects_dir);
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOdb::New((void *)out);
  return scope.Close(to);
}

/**
 * @param {String} path
 */
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
  free((void *)from_path);
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  return Undefined();
}

/**
 * @param {Oid} id
 * @param {OdbObject} callback
 */
Handle<Value> GitOdb::Read(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid id is required.")));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  ReadBaton* baton = new ReadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->dbReference = Persistent<Value>::New(args.This());
  baton->db = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
  baton->idReference = Persistent<Value>::New(args[0]);
    const git_oid * from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadWork, (uv_after_work_cb)ReadAfterWork);

  return Undefined();
}

void GitOdb::ReadWork(uv_work_t *req) {
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);
  int result = git_odb_read(
    &baton->out, 
    baton->db, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitOdb::ReadAfterWork(uv_work_t *req) {
  HandleScope scope;
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOdbObject::New((void *)baton->out);
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
  baton->dbReference.Dispose();
  baton->idReference.Dispose();
  baton->callback.Dispose();
  delete baton;
}

/**
 * @param {Odb} db
 * @param {Oid} short_id
 * @param {Number} len
 * @return {OdbObject} out
 */
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

/**
 * @param {Number} len_out
 * @param {Number} type_out
 * @param {Odb} db
 * @param {Oid} id
 */
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

/**
 * @param {Oid} id
 */
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

/**
 */
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

/**
 * @param {String} data
 * @param {Number} len
 * @param {Number} type
 * @param {Oid} callback
 */
Handle<Value> GitOdb::Write(const Arguments& args) {
  HandleScope scope;
      if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String data is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number len is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number type is required.")));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return ThrowException(Exception::Error(String::New("Callback is required and must be a Function.")));
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  baton->odbReference = Persistent<Value>::New(args.This());
  baton->odb = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
  baton->dataReference = Persistent<Value>::New(args[0]);
    String::Utf8Value data(args[0]->ToString());
  const void * from_data = strdup(*data);
  baton->data = from_data;
  baton->lenReference = Persistent<Value>::New(args[1]);
    size_t from_len = (size_t) args[1]->ToUint32()->Value();
  baton->len = from_len;
  baton->typeReference = Persistent<Value>::New(args[2]);
    git_otype from_type = (git_otype) args[2]->ToInt32()->Value();
  baton->type = from_type;
  baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteWork, (uv_after_work_cb)WriteAfterWork);

  return Undefined();
}

void GitOdb::WriteWork(uv_work_t *req) {
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);
  int result = git_odb_write(
    baton->out, 
    baton->odb, 
    baton->data, 
    baton->len, 
    baton->type
  );
  baton->error_code = result;
  if (result != GIT_OK) {
    baton->error = giterr_last();
  }
}

void GitOdb::WriteAfterWork(uv_work_t *req) {
  HandleScope scope;
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    to = GitOid::New((void *)baton->out);
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
  baton->odbReference.Dispose();
  baton->dataReference.Dispose();
  baton->lenReference.Dispose();
  baton->typeReference.Dispose();
  baton->callback.Dispose();
  free((void *)baton->data);
  delete baton;
}

/**
 * @param {Buffer} data
 * @param {Number} len
 * @param {Number} type
 * @return {Oid} out
 */
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

/**
 * @param {String} path
 * @param {Number} type
 * @return {Oid} out
 */
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
  free((void *)from_path);
  if (result != GIT_OK) {
    return ThrowException(Exception::Error(String::New(giterr_last()->message)));
  }

  Handle<Value> to;
    to = GitOid::New((void *)out);
  return scope.Close(to);
}

Persistent<Function> GitOdb::constructor_template;
