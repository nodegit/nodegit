/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string>

#include "../include/wrapper.h"
#include "node_buffer.h"

using namespace v8;
using namespace node;

Wrapper::Wrapper(void *raw) {
  this->raw = raw;
}

void Wrapper::Initialize(Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Wrapper"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "toBuffer", ToBuffer);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Wrapper"), constructor_template);
}

Handle<Value> Wrapper::New(const Arguments& args) {
  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return ThrowException(Exception::Error(String::New("void * is required.")));
  }

  Wrapper* object = new Wrapper(External::Unwrap(args[0]));
  object->Wrap(args.This());

  return scope.Close(args.This());
}

void *Wrapper::GetValue() {
  return this->raw;
}

Handle<Value> Wrapper::ToBuffer(const Arguments& args) {
  HandleScope scope;

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return ThrowException(Exception::Error(String::New("Number is required.")));
  }

  int len = args[0]->ToNumber()->Value();

  Buffer *slowBuffer = Buffer::New(
    const_cast<char *>(std::string((const char *)const_cast<void *>(ObjectWrap::Unwrap<Wrapper>(args.This())->GetValue())).c_str()),
    len);

  Local<Function> bufferConstructor = Local<Function>::Cast(
    Context::GetCurrent()->Global()->Get(String::New("Buffer"))); 

  Handle<Value> constructorArgs[3] = { slowBuffer->handle_, Integer::New(len), Integer::New(0) };
  Local<Object> fastBuffer = bufferConstructor->NewInstance(3, constructorArgs);

  return scope.Close(fastBuffer);
}


Persistent<Function> Wrapper::constructor_template;
