/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Odb"));

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


  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("Odb"), _constructor_template);
}

NAN_METHOD(GitOdb::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_odb is required.");
  }
  GitOdb* object = new GitOdb(static_cast<git_odb *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitOdb::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitOdb::constructor_template)->NewInstance(1, argv));
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdb::New((void *)out);
  } else {
    to = NanNull();
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
    return NanThrowError("String objects_dir is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdb::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

/**
 * @param {String} path
 */
NAN_METHOD(GitOdb::AddDiskAlternate) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsString()) {
    return NanThrowError("String path is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
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
    return NanThrowError("Oid id is required.");
  }

  if (args.Length() == 1 || !args[1]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  ReadBaton* baton = new ReadBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->db = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
    const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
          baton->id = from_id;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[1]));
  ReadWorker *worker = new ReadWorker(baton, callback);
  worker->SaveToPersistent("db", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("id", args[0]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitOdb::ReadWorker::Execute() {
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

void GitOdb::ReadWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOdbObject::New((void *)baton->out);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
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
 * @param {Odb} db
 * @param {Oid} short_id
 * @param {Number} len
 * @return {OdbObject} out
 */
NAN_METHOD(GitOdb::ReadPrefix) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Odb db is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Oid short_id is required.");
  }
  if (args.Length() == 2 || !args[2]->IsUint32()) {
    return NanThrowError("Number len is required.");
  }

  git_odb_object * out = 0;
  git_odb * from_db;
            from_db = ObjectWrap::Unwrap<GitOdb>(args[0]->ToObject())->GetValue();
        const git_oid * from_short_id;
            from_short_id = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject())->GetValue();
        size_t from_len;
            from_len = (size_t)   args[2]->ToUint32()->Value();
      
  int result = git_odb_read_prefix(
    &out
    , from_db
    , from_short_id
    , from_len
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOdbObject::New((void *)out);
  } else {
    to = NanNull();
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
    return NanThrowError("Odb db is required.");
  }
  if (args.Length() == 1 || !args[1]->IsObject()) {
    return NanThrowError("Oid id is required.");
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
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Object> toReturn = NanNew<Object>();
  Handle<Value> to;
      to = NanNew<Uint32>((uint32_t)len_out);
    toReturn->Set(NanNew<String>("len_out"), to);

      to = NanNew<Int32>((int32_t)type_out);
    toReturn->Set(NanNew<String>("type_out"), to);

  NanReturnValue(toReturn);
}

/**
 * @param {Oid} id
 */
NAN_METHOD(GitOdb::Exists) {
  NanScope();
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return NanThrowError("Oid id is required.");
  }

  const git_oid * from_id;
            from_id = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject())->GetValue();
      
  int result = git_odb_exists(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
    , from_id
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
NAN_METHOD(GitOdb::Refresh) {
  NanScope();
  

  int result = git_odb_refresh(
    ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue()
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
    return NanThrowError("String data is required.");
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return NanThrowError("Number len is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number type is required.");
  }

  if (args.Length() == 3 || !args[3]->IsFunction()) {
    return NanThrowError("Callback is required and must be a Function.");
  }

  WriteBaton* baton = new WriteBaton;
  baton->error_code = GIT_OK;
  baton->error = NULL;
  baton->out = (git_oid *)malloc(sizeof(git_oid ));
  baton->odb = ObjectWrap::Unwrap<GitOdb>(args.This())->GetValue();
    const void * from_data;
            String::Utf8Value data(args[0]->ToString());
      from_data = strdup(*data);
          baton->data = from_data;
      size_t from_len;
            from_len = (size_t)   args[1]->ToUint32()->Value();
          baton->len = from_len;
      git_otype from_type;
            from_type = (git_otype)   args[2]->ToInt32()->Value();
          baton->type = from_type;
  
  NanCallback *callback = new NanCallback(Local<Function>::Cast(args[3]));
  WriteWorker *worker = new WriteWorker(baton, callback);
  worker->SaveToPersistent("odb", args.This());
  if (!args[0]->IsUndefined() && !args[0]->IsNull())
    worker->SaveToPersistent("data", args[0]->ToObject());
  if (!args[1]->IsUndefined() && !args[1]->IsNull())
    worker->SaveToPersistent("len", args[1]->ToObject());
  if (!args[2]->IsUndefined() && !args[2]->IsNull())
    worker->SaveToPersistent("type", args[2]->ToObject());

  NanAsyncQueueWorker(worker);
  NanReturnUndefined();
}

void GitOdb::WriteWorker::Execute() {
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

void GitOdb::WriteWorker::HandleOKCallback() {
  TryCatch try_catch;
  if (baton->error_code == GIT_OK) {
  Handle<Value> to;
    if (baton->out != NULL) {
    to = GitOid::New((void *)baton->out);
  } else {
    to = NanNull();
  }
  Handle<Value> result = to;
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
        free(baton->out);
      }

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
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
    return NanThrowError("Buffer data is required.");
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return NanThrowError("Number len is required.");
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return NanThrowError("Number type is required.");
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid));
  const void * from_data;
            from_data = Buffer::Data(args[0]->ToObject());
        size_t from_len;
            from_len = (size_t)   args[1]->ToUint32()->Value();
        git_otype from_type;
            from_type = (git_otype)   args[2]->ToInt32()->Value();
      
  int result = git_odb_hash(
    out
    , from_data
    , from_len
    , from_type
  );
  if (result != GIT_OK) {
    free(out);
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = NanNull();
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
    return NanThrowError("String path is required.");
  }
  if (args.Length() == 1 || !args[1]->IsInt32()) {
    return NanThrowError("Number type is required.");
  }

  git_oid *out = (git_oid *)malloc(sizeof(git_oid));
  const char * from_path;
            String::Utf8Value path(args[0]->ToString());
      from_path = strdup(*path);
        git_otype from_type;
            from_type = (git_otype)   args[1]->ToInt32()->Value();
      
  int result = git_odb_hashfile(
    out
    , from_path
    , from_type
  );
  free((void *)from_path);
  if (result != GIT_OK) {
    free(out);
    if (giterr_last()) {
      return NanThrowError(giterr_last()->message);
    } else {
      return NanThrowError("Unknown Error");
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitOid::New((void *)out);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

Persistent<Function> GitOdb::constructor_template;
