/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "../include/functions/copy.h"

#include "../include/index_entry.h"
#include "../include/index_time.h"

using namespace v8;
using namespace node;

GitIndexEntry::GitIndexEntry(git_index_entry *raw) {
  this->raw = raw;
}

GitIndexEntry::~GitIndexEntry() {
  free(this->raw);
}

void GitIndexEntry::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("IndexEntry"));


  NODE_SET_PROTOTYPE_METHOD(tpl, "ctime", Ctime);
  NODE_SET_PROTOTYPE_METHOD(tpl, "mtime", Mtime);
  NODE_SET_PROTOTYPE_METHOD(tpl, "path", Path);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("IndexEntry"), constructor_template);
}

Handle<Value> GitIndexEntry::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("git_index_entry is required.")));
  }

  GitIndexEntry* object = new GitIndexEntry((git_index_entry *) External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitIndexEntry::New(void *raw) {
  HandleScope scope;
  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(GitIndexEntry::constructor_template->NewInstance(1, argv));
}

git_index_entry *GitIndexEntry::GetValue() {
  return this->raw;
}


Handle<Value> GitIndexEntry::Ctime(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_index_time *ctime =
    &ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->ctime;

    ctime = (git_index_time *)git_index_time_dup(ctime);
  to = GitIndexTime::New((void *)ctime);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Mtime(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  git_index_time *mtime =
    &ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->mtime;

    mtime = (git_index_time *)git_index_time_dup(mtime);
  to = GitIndexTime::New((void *)mtime);
  return scope.Close(to);
}

Handle<Value> GitIndexEntry::Path(const Arguments& args) {
  HandleScope scope;
    Handle<Value> to;

  char * path =
    ObjectWrap::Unwrap<GitIndexEntry>(args.This())->GetValue()->path;

    to = String::New(path);
  return scope.Close(to);
}

Persistent<Function> GitIndexEntry::constructor_template;
