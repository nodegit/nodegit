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
  NanScope();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanSymbol("Odb"));

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

  NanAssignPersistent(FunctionTemplate, constructor_template, tpl);
  target->Set(String::NewSymbol("Odb"), tpl->GetFunction());
}

NAN_METHOD(GitOdb::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError(String::New("git_odb is required."));
  }

  GitOdb* object = new GitOdb((git_odb *) External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitOdb::New(void *raw) {
  NanScope();
  Handle<Value> argv[1] = { External::New((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanPersistentToLocal(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  return scope.Close(instance);
}

git_odb *GitOdb::GetValue() {
  return this->raw;
}


/**
 * @return {Odb} out
 */
NAN_METHOD(GitOdb::Create) {
  NanScope();
  
  git_odb * out = 0;

  int result = git_odb_new(
    &out
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdb::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} objects_dir
 * @return {Odb} out
 */
NAN_METHOD(GitOdb::Open) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String objects_dir is required."));
  }

  git_odb * out = 0;
  const char * from_objects_dir;
            String::Utf8Value objects_dir(args[0]->ToString());
      from_objects_dir = strdup(*objects_dir);
      
  int result = git_odb_open(
    &out
    , from_objects_dir
  );
  free((void *)from_objects_dir);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdb::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} path
 */
NAN_METHOD(GitOdb::AddDiskAlternate) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }

  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
      
  int result = git_odb_add_disk_alternate(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_path
  );
  free((void *)from_path);
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
 * @param {Oid} id
 * @param {OdbObject} callback
 */
NAN_METHOD(GitOdb::Read) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Oid id is required."));
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  ReadBaton* baton = new ReadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  NanAssignPersistent(Value, baton->dbReference, args.This());
  baton->db = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->idReference, args[0]);
  const git_oid * from_id;
  from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
  baton->id = from_id;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[1]));

  uv_queue_work(uv_default_loop(), &baton->request, ReadWork, (uv_after_work_cb)ReadAfterWork);

  NanReturnUndefined();
}

void GitOdb::ReadWork(uv_work_t *req) {
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);
  int result = git_odb_read(
    &baton->out, 
    baton->db, 
    baton->id
  );
  baton->error_code = result;
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitOdb::ReadAfterWork(uv_work_t *req) {
  NanScope();
  ReadBaton *baton = static_cast<ReadBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOdbObject::New((void *)baton->out);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
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
NAN_METHOD(GitOdb::ReadPrefix) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Odb db is required."));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError(String::New("Oid short_id is required."));
  }
  if (args.Length() == 2 || !args[2]->IsUint32()) {
    return NanThrowError(String::New("Number len is required."));
  }

  git_odb_object * out = 0;
  git_odb * from_db;
            from_db = ObjectWrap::Unwrap<GitOdb>(args[0]->ToObject())->GetValue();
        const git_oid * from_short_id;
            from_short_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
        size_t from_len;
            from_len = (size_t) args[2]->ToUint32()->Value();
      
  int result = git_odb_read_prefix(
    &out
    , from_db
    , from_short_id
    , from_len
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdbObject::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {Odb} db
 * @param {Oid} id
 * @return {Number} len_out
 * @return {Number} type_out
 */
NAN_METHOD(GitOdb::ReadHeader) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Odb db is required."));
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError(String::New("Oid id is required."));
  }

  size_t len_out = 0;
  git_otype type_out = GIT_OBJ_ANY;
  git_odb * from_db;
            from_db = ObjectWrap::Unwrap<GitOdb>(args[0]->ToObject())->GetValue();
        const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
      
  int result = git_odb_read_header(
    &len_out
    , &type_out
    , from_db
    , from_id
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Object> toReturn = Object::New();
  Handle<Value> to;
      to = Uint32::New(len_out);
    toReturn->Set(String::NewSymbol("len_out"), to);

      to = Int32::New(type_out);
    toReturn->Set(String::NewSymbol("type_out"), to);

  NanReturnValue(toReturn);
}

/**
 * @param {Oid} id
 */
NAN_METHOD(GitOdb::Exists) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Oid id is required."));
  }

  const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
      
  int result = git_odb_exists(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_id
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
NAN_METHOD(GitOdb::Refresh) {
  NanScope();
  

  int result = git_odb_refresh(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
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

#include "../include/functions/copy.h"

/**
 * @param {String} data
 * @param {Number} len
 * @param {Number} type
 * @param {Oid} callback
 */
NAN_METHOD(GitOdb::Write) {
  NanScope();
      if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String data is required."));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return NanThrowError(String::New("Number len is required."));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError(String::New("Number type is required."));
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError(String::New("Callback is required and must be a Function."));
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->request.data = baton;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  NanAssignPersistent(Value, baton->odbReference, args.This());
  baton->odb = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
  NanAssignPersistent(Value, baton->dataReference, args[0]);
  const void * from_data;
  String::Utf8Value data(args[0]->ToString());
  from_data = strdup(*data);
  baton->data = from_data;
  NanAssignPersistent(Value, baton->lenReference, args[1]);
  size_t from_len;
  from_len = (size_t) args[1]->ToUint32()->Value();
  baton->len = from_len;
  NanAssignPersistent(Value, baton->typeReference, args[2]);
  git_otype from_type;
  from_type = (git_otype) args[2]->ToInt32()->Value();
  baton->type = from_type;
  NanAssignPersistent(Function, baton->callback, Local<Function>::Cast(args[3]));

  uv_queue_work(uv_default_loop(), &baton->request, WriteWork, (uv_after_work_cb)WriteAfterWork);

  NanReturnUndefined();
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
  if (result != GIT_OK && giterr_last() != NULL) {
    baton->error = git_error_dup(giterr_last());
  }
}

void GitOdb::WriteAfterWork(uv_work_t *req) {
  NanScope();
  WriteBaton *baton = static_cast<WriteBaton *>(req->data);

  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOid::New((void *)baton->out);
  } else {
    to = Null();
  }
  Handle<Value> result = to;
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
        free(baton->out);
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
NAN_METHOD(GitOdb::Hash) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError(String::New("Buffer data is required."));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return NanThrowError(String::New("Number len is required."));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError(String::New("Number type is required."));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid));
  const void * from_data;
            from_data = Buffer::Data(args[0]->ToObject());
        size_t from_len;
            from_len = (size_t) args[1]->ToUint32()->Value();
        git_otype from_type;
            from_type = (git_otype) args[2]->ToInt32()->Value();
      
  int result = git_odb_hash(
    out
    , from_data
    , from_len
    , from_type
  );
  if (result != GIT_OK) {
    free(out);
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

/**
 * @param {String} path
 * @param {Number} type
 * @return {Oid} out
 */
NAN_METHOD(GitOdb::Hashfile) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError(String::New("String path is required."));
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError(String::New("Number type is required."));
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid));
  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
        git_otype from_type;
            from_type = (git_otype) args[1]->ToInt32()->Value();
      
  int result = git_odb_hashfile(
    out
    , from_path
    , from_type
  );
  free((void *)from_path);
  if (result != GIT_OK) {
    free(out);
    if (giterr_last()) {
      return NanThrowError(String::New(giterr_last()->message));
    } else {
      return NanThrowError(String::New("Unkown Error"));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = Null();
  }
  NanReturnValue(to);
}

Persistent<FunctionTemplate> GitOdb::constructor_template;
