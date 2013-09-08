/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/branch.h"

using namespace v8;
using namespace node;

Branch::Branch(git_branch *raw) {
  this->raw = raw;
}

Branch::~Branch() {
  git_branch_free(this->raw);
}

void Branch::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Branch"));

  NODE_SET_METHOD(tpl, "create", Create);
  NODE_SET_METHOD(tpl, "delete", Delete);
  NODE_SET_METHOD(tpl, "foreach", Foreach);
  NODE_SET_METHOD(tpl, "move", Move);
  NODE_SET_METHOD(tpl, "lookup", Lookup);
  NODE_SET_METHOD(tpl, "name", Name);
  NODE_SET_METHOD(tpl, "upstream", Upstream);
  NODE_SET_METHOD(tpl, "setUpstream", SetUpstream);
  NODE_SET_METHOD(tpl, "upstreamName", UpstreamName);
  NODE_SET_METHOD(tpl, "isHead", IsHead);
  NODE_SET_METHOD(tpl, "remoteName", RemoteName);


  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Branch"), constructor_template);
}

Handle<Value> Branch::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_branch is required.")));
  }

  Branch* object = new Branch((git_branch *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> Branch::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(Branch::constructor_template->NewInstance(1, argv));
}

git_branch *Branch::GetValue() {
  return this->raw;
}


/**
 * @param {Repository} repo
 * @param {String} branch_name
 * @param {Commit} target
 * @param {Number} force
 * @return {Reference} out
 */
Handle<Value> Branch::Create(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Commit target is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference * out = 0;
  git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
        const char * from_branch_name;
            String::Utf8Value branch_name(args[1]->ToString());
      from_branch_name = strdup(*branch_name);
        const git_commit * from_target;
            from_target = ObjectWrap::Unwrap<GitCommit>(args[2]->ToObject())->GetValue();
        int from_force;
            from_force = (int) args[3]->ToInt32()->Value();
      
  int result = git_branch_create(
    &out
    , from_repo
    , from_branch_name
    , from_target
    , from_force
  );
  free((void *)from_branch_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Reference} branch
 */
Handle<Value> Branch::Delete(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }

  git_reference * from_branch;
            from_branch = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
      
  int result = git_branch_delete(
    from_branch
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
 * @param {Repository} repo
 * @param {Number} list_flags
 * @param {BranchForeachCb} branch_cb
 * @param {void} payload
 */
Handle<Value> Branch::Foreach(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number list_flags is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("BranchForeachCb branch_cb is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsObject()) {
    return ThrowException(Exception::Error(String::New("void payload is required.")));
  }

  git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
        unsigned int from_list_flags;
            from_list_flags = (unsigned int) args[1]->ToUint32()->Value();
        git_branch_foreach_cb from_branch_cb;
            from_branch_cb = ObjectWrap::Unwrap<BranchForeachCb>(args[2]->ToObject())->GetValue();
        void * from_payload;
            from_payload = ObjectWrap::Unwrap<void>(args[3]->ToObject())->GetValue();
      
  int result = git_branch_foreach(
    from_repo
    , from_list_flags
    , from_branch_cb
    , from_payload
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
 * @param {Reference} branch
 * @param {String} new_branch_name
 * @param {Number} force
 * @return {Reference} out
 */
Handle<Value> Branch::Move(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String new_branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsInt32()) {
    return ThrowException(Exception::Error(String::New("Number force is required.")));
  }

  git_reference * out = 0;
  git_reference * from_branch;
            from_branch = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
        const char * from_new_branch_name;
            String::Utf8Value new_branch_name(args[1]->ToString());
      from_new_branch_name = strdup(*new_branch_name);
        int from_force;
            from_force = (int) args[2]->ToInt32()->Value();
      
  int result = git_branch_move(
    &out
    , from_branch
    , from_new_branch_name
    , from_force
  );
  free((void *)from_new_branch_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Repository} repo
 * @param {String} branch_name
 * @param {BranchT} branch_type
 * @return {Reference} out
 */
Handle<Value> Branch::Lookup(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String branch_name is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("BranchT branch_type is required.")));
  }

  git_reference * out = 0;
  git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject())->GetValue();
        const char * from_branch_name;
            String::Utf8Value branch_name(args[1]->ToString());
      from_branch_name = strdup(*branch_name);
        git_branch_t from_branch_type;
            from_branch_type = ObjectWrap::Unwrap<BranchT>(args[2]->ToObject())->GetValue();
      
  int result = git_branch_lookup(
    &out
    , from_repo
    , from_branch_name
    , from_branch_type
  );
  free((void *)from_branch_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Reference} ref
 * @return {String} out
 */
Handle<Value> Branch::Name(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference ref is required.")));
  }

  const char * out = 0;
  git_reference * from_ref;
            from_ref = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
      
  int result = git_branch_name(
    &out
    , from_ref
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    to = String::New(out);
  return scope.Close(to);
}

/**
 * @param {Reference} branch
 * @return {Reference} out
 */
Handle<Value> Branch::Upstream(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }

  git_reference * out = 0;
  git_reference * from_branch;
            from_branch = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
      
  int result = git_branch_upstream(
    &out
    , from_branch
  );
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  Handle<Value> to;
    if (out != NULL) {
    to = GitReference::New((void *)out);
  } else {
    to = Null();
  }
  return scope.Close(to);
}

/**
 * @param {Reference} branch
 * @param {String} upstream_name
 */
Handle<Value> Branch::SetUpstream(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsString()) {
    return ThrowException(Exception::Error(String::New("String upstream_name is required.")));
  }

  git_reference * from_branch;
            from_branch = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
        const char * from_upstream_name;
            String::Utf8Value upstream_name(args[1]->ToString());
      from_upstream_name = strdup(*upstream_name);
      
  int result = git_branch_set_upstream(
    from_branch
    , from_upstream_name
  );
  free((void *)from_upstream_name);
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
 * @param {String} tracking_branch_name_out
 * @param {Number} buffer_size
 * @param {Repository} repo
 * @param {String} canonical_branch_name
 */
Handle<Value> Branch::UpstreamName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String tracking_branch_name_out is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number buffer_size is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return ThrowException(Exception::Error(String::New("String canonical_branch_name is required.")));
  }

  char * from_tracking_branch_name_out;
            String::Utf8Value tracking_branch_name_out(args[0]->ToString());
      from_tracking_branch_name_out = strdup(*tracking_branch_name_out);
        size_t from_buffer_size;
            from_buffer_size = (size_t) args[1]->ToUint32()->Value();
        git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[2]->ToObject())->GetValue();
        const char * from_canonical_branch_name;
            String::Utf8Value canonical_branch_name(args[3]->ToString());
      from_canonical_branch_name = strdup(*canonical_branch_name);
      
  int result = git_branch_upstream_name(
    from_tracking_branch_name_out
    , from_buffer_size
    , from_repo
    , from_canonical_branch_name
  );
  free((void *)from_tracking_branch_name_out);
  free((void *)from_canonical_branch_name);
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
 * @param {Reference} branch
 */
Handle<Value> Branch::IsHead(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Reference branch is required.")));
  }

  git_reference * from_branch;
            from_branch = ObjectWrap::Unwrap<GitReference>(args[0]->ToObject())->GetValue();
      
  int result = git_branch_is_head(
    from_branch
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
 * @param {String} remote_name_out
 * @param {Number} buffer_size
 * @param {Repository} repo
 * @param {String} canonical_branch_name
 */
Handle<Value> Branch::RemoteName(const Arguments& args) {
  HandleScope scope;
    if (args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("String remote_name_out is required.")));
  }
  if (args.Length() == 1 || !args[1]->IsUint32()) {
    return ThrowException(Exception::Error(String::New("Number buffer_size is required.")));
  }
  if (args.Length() == 2 || !args[2]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repository repo is required.")));
  }
  if (args.Length() == 3 || !args[3]->IsString()) {
    return ThrowException(Exception::Error(String::New("String canonical_branch_name is required.")));
  }

  char * from_remote_name_out;
            String::Utf8Value remote_name_out(args[0]->ToString());
      from_remote_name_out = strdup(*remote_name_out);
        size_t from_buffer_size;
            from_buffer_size = (size_t) args[1]->ToUint32()->Value();
        git_repository * from_repo;
            from_repo = ObjectWrap::Unwrap<GitRepo>(args[2]->ToObject())->GetValue();
        const char * from_canonical_branch_name;
            String::Utf8Value canonical_branch_name(args[3]->ToString());
      from_canonical_branch_name = strdup(*canonical_branch_name);
      
  int result = git_branch_remote_name(
    from_remote_name_out
    , from_buffer_size
    , from_repo
    , from_canonical_branch_name
  );
  free((void *)from_remote_name_out);
  free((void *)from_canonical_branch_name);
  if (result != GIT_OK) {
    if (giterr_last()) {
      return ThrowException(Exception::Error(String::New(giterr_last()->message)));
    } else {
      return ThrowException(Exception::Error(String::New("Unkown Error")));
    }
  }

  return Undefined();
}

Persistent<Function> Branch::constructor_template;
