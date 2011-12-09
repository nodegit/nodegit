/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/oid.h"

using namespace v8;
using namespace node;

void GitOid::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Oid"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "mkstr", Mkstr);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "mkraw", Mkraw);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "fmt", Fmt);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "pathFmt", PathFmt);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "allocFmt", AllocFmt);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "toString", ToString);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "cpy", Cpy);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "cmp", Cmp);

  target->Set(String::NewSymbol("Oid"), constructor_template->GetFunction());
}

git_oid GitOid::GetValue() {
  return this->oid;
}

void GitOid::SetValue(git_oid oid) {
  this->oid = oid;
}

int GitOid::Mkstr(const char* id) {
  return git_oid_fromstr(&this->oid, id);
}

void GitOid::Mkraw(const unsigned char* raw) {
  git_oid_fromraw(&this->oid, raw);
}

void GitOid::Fmt(char* buffer) {
  git_oid_fmt(buffer, &this->oid);
}

void GitOid::PathFmt(char* str) {
  git_oid_pathfmt(str, &this->oid);
}

char* GitOid::AllocFmt() {
  return git_oid_allocfmt(&this->oid);
}

char* GitOid::ToString(char* buffer, size_t bufferSize) {
  git_oid_to_string(buffer, bufferSize, &this->oid);
}

void GitOid::Cpy(git_oid* out) {
  git_oid_cpy(out, &this->oid);
}

int GitOid::Cmp(const git_oid* a, const git_oid* b) {
  return git_oid_cmp(a, b);
}

Handle<Value> GitOid::New(const Arguments& args) {
  HandleScope scope;

  GitOid* oid = new GitOid();
  oid->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitOid::Mkstr(const Arguments& args) {
  HandleScope scope;

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args.This());

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Object id is required and must be a hex formatted String.")));
  }

  String::Utf8Value id(args[0]);

  return scope.Close(Integer::New(oid->Mkstr(*id)));
}

Handle<Value> GitOid::Mkraw(const Arguments& args) {
  HandleScope scope;

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args.This());

  if(args.Length() == 0 || !args[0]->IsString()) {
    return ThrowException(Exception::Error(String::New("Raw object id is required.")));
  }

  String::Utf8Value raw(args[0]);
  oid->Mkraw((const unsigned char*)*raw);

  return scope.Close( args.This() );
}

Handle<Value> GitOid::Fmt(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());

  char buffer[40];
  oid->Fmt(buffer);

  return scope.Close( String::New(buffer) );
}

Handle<Value> GitOid::PathFmt(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());

  char buffer[41];
  oid->PathFmt(buffer);

  return scope.Close( String::New(buffer) );
}

Handle<Value> GitOid::AllocFmt(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());

  return scope.Close( String::New(oid->AllocFmt()) );
}

Handle<Value> GitOid::ToString(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Length argument is required and must be a Number.")));
  }

  char buffer[Int32::Cast(*args[0])->Value()+1];
  oid->ToString(buffer, sizeof(buffer));

  return scope.Close( String::New(buffer) );
}

Handle<Value> GitOid::Cpy(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitOid argument is required and must be a Object.")));
  }

  GitOid *clone = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  
  git_oid *out;
  oid->Cpy(out);
  clone->SetValue(*out);

  return scope.Close( Undefined() );
}

Handle<Value> GitOid::Cmp(const Arguments& args) {
  HandleScope scope;

  GitOid *oid = ObjectWrap::Unwrap<GitOid>(args.This());
  
  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitOid argument is required and must be a Object.")));
  }
  
  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("GitOid argument is required and must be a Object.")));
  }

  GitOid* a = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());
  GitOid* b = ObjectWrap::Unwrap<GitOid>(args[1]->ToObject());

  //int cmp = oid->Cmp(&a->GetValue(), &b->GetValue());
  int cmp = 0;

  return scope.Close( Integer::New(cmp) );
}
Persistent<FunctionTemplate> GitOid::constructor_template;
