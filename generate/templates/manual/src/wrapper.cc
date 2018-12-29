/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/
#include <nan.h>
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

void Wrapper::InitializeComponent(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("Wrapper").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "toBuffer", ToBuffer);

  constructor_template.Reset(tpl);
  Nan::Set(target, Nan::New("Wrapper").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Wrapper::JSNewFunction) {

  if (info.Length() == 0 || !info[0]->IsExternal()) {
    return Nan::ThrowError("void * is required.");
  }

  Wrapper* object = new Wrapper(External::Cast(*info[0])->Value());
  object->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

Local<v8::Value> Wrapper::New(const void *raw) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> argv[1] = { Nan::New<External>((void *)raw) };
  Local<Object> instance;
  Local<FunctionTemplate> constructorHandle = Nan::New(constructor_template);
  instance = Nan::NewInstance(Nan::GetFunction(constructorHandle).ToLocalChecked(), 1, argv).ToLocalChecked();

  return scope.Escape(instance);
}

void *Wrapper::GetValue() {
  return this->raw;
}

NAN_METHOD(Wrapper::ToBuffer) {

  if(info.Length() == 0 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Number is required.");
  }

  int len = Nan::To<int>(info[0]).FromJust();

  Local<Function> bufferConstructor = Local<Function>::Cast(
    Nan::Get(Nan::GetCurrentContext()->Global(), Nan::New("Buffer").ToLocalChecked()).ToLocalChecked());

  Local<v8::Value> constructorArgs[1] = { Nan::New(len) };
  Local<Object> nodeBuffer = Nan::NewInstance(bufferConstructor, 1, constructorArgs).ToLocalChecked();

  std::memcpy(node::Buffer::Data(nodeBuffer), Nan::ObjectWrap::Unwrap<Wrapper>(info.This())->GetValue(), len);

  info.GetReturnValue().Set(nodeBuffer);
}


Nan::Persistent<FunctionTemplate> Wrapper::constructor_template;
