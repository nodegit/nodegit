/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <v8.h>
#include <node.h>
#include <string>
#include <cstring>

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

Handle<Value> Wrapper::New(void *raw) {
  HandleScope scope;

  Handle<Value> argv[1] = { External::New((void *)raw) };
  return scope.Close(Wrapper::constructor_template->NewInstance(1, argv));
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

  Local<Function> bufferConstructor = Local<Function>::Cast(
    Context::GetCurrent()->Global()->Get(String::New("Buffer"))); 

  Handle<Value> constructorArgs[1] = { Integer::New(len) };
  Local<Object> nodeBuffer = bufferConstructor->NewInstance(1, constructorArgs);

  std::memcpy(node::Buffer::Data(nodeBuffer), ObjectWrap::Unwrap<Wrapper>(args.This())->GetValue(), len);

  return scope.Close(nodeBuffer);
}


Persistent<Function> Wrapper::constructor_template;
