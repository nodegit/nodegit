/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("Wrapper"));
  
  NODE_SET_PROTOTYPE_METHOD(tpl, "toBuffer", ToBuffer);

  NanAssignPersistent(constructor_template, tpl);
  target->Set(NanNew<String>("Wrapper"), tpl->GetFunction());
}

NAN_METHOD(Wrapper::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("void * is required.");
  }

  Wrapper* object = new Wrapper(External::Cast(*args[0])->Value());
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> Wrapper::New(void *raw) {
  NanEscapableScope();
  
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = NanNew(constructor_template);
  instance = constructorHandle->GetFunction()->NewInstance(1, argv);
  
  return NanEscapeScope(instance);
}

void *Wrapper::GetValue() {
  return this->raw;
}

NAN_METHOD(Wrapper::ToBuffer) {
  NanScope();

  if(args.Length() == 0 || !args[0]->IsNumber()) {
    return NanThrowError("Number is required.");
  }

  int len = args[0]->ToNumber()->Value();

  Local<Function> bufferConstructor = Local<Function>::Cast(
    NanGetCurrentContext()->Global()->Get(NanNew<String>("Buffer"))); 

  Handle<Value> constructorArgs[1] = { NanNew<Integer>(len) };
  Local<Object> nodeBuffer = bufferConstructor->NewInstance(1, constructorArgs);

  std::memcpy(node::Buffer::Data(nodeBuffer), ObjectWrap::Unwrap<Wrapper>(args.This())->GetValue(), len);

  NanReturnValue(nodeBuffer);
}


Persistent<FunctionTemplate> Wrapper::constructor_template;
