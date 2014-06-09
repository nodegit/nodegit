/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/diff_file.h"
#include "../include/oid.h"

using namespace v8;
using namespace node;

GitDiffFile::GitDiffFile(git_diff_file *raw) {
  this->raw = raw;
}

GitDiffFile::~GitDiffFile() {
  free(this->raw);
}

void GitDiffFile::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("DiffFile"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "oid", Oid);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);
  NODE_SET_PROTOTYPE_METHOD(tpl, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(tpl, "flags", Flags);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mode", Mode);

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("DiffFile"), _constructor_template);
}

NAN_METHOD(GitDiffFile::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("git_diff_file is required.");
  }
  GitDiffFile* object = new GitDiffFile(static_cast<git_diff_file *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> GitDiffFile::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>(GitDiffFile::constructor_template)->NewInstance(1, argv));
}

git_diff_file *GitDiffFile::GetValue() {
  return this->raw;
}


NAN_METHOD(GitDiffFile::Oid) {
  NanScope();
    Handle<Value> to;

  git_oid *oid =
    &ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->oid;

    if (oid != NULL) {
    oid = (git_oid *)git_oid_dup(oid);
  }
  if (oid != NULL) {
    to = GitOid::New((void *)oid);
  } else {
    to = NanNull();
  }
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Path) {
  NanScope();
    Handle<Value> to;

  const char * path =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->path;

    to = NanNew<String>(path);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Size) {
  NanScope();
    Handle<Value> to;

  git_off_t size =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->size;

    to = NanNew<Integer>(size);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Flags) {
  NanScope();
    Handle<Value> to;

  uint32_t flags =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->flags;

    to = NanNew<Integer>(flags);
  NanReturnValue(to);
}

NAN_METHOD(GitDiffFile::Mode) {
  NanScope();
    Handle<Value> to;

  uint16_t mode =
    ObjectWrap::Unwrap<GitDiffFile>(args.This())->GetValue()->mode;

    to = NanNew<Integer>(mode);
  NanReturnValue(to);
}

Persistent<Function> GitDiffFile::constructor_template;
