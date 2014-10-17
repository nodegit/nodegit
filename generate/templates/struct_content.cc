// This is a generated file, modify: generate/templates/class.cc.
#include <nan.h>
#include <string.h>
#include <iostream>

extern "C" {
#include <git2.h>
}

#include "../include/functions/copy.h"
#include "../include/{{ filename }}"

{%each dependencies as dependency%}
#include "{{ dependency }}"
{%endeach%}

using namespace v8;
using namespace node;
using namespace std;

{{ cppClassName }}::{{ cppClassName }}() {
  {{ cType }} wrappedValue = {{ cType|upper }}_INIT;
  this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}));
  memcpy(this->raw, &wrappedValue, sizeof({{ cType }}));

  this->ConstructFields();
  this->selfFreeing = true;
}

{{ cppClassName }}::{{ cppClassName }}({{ cType }}* raw) {
  this->raw = raw;
  this->ConstructFields();
  this->selfFreeing = true;
}

{{ cppClassName }}::~{{ cppClassName }}() {
  // This is going to cause memory leaks. We'll have to solve that later
  // TODO: Clean up memory better
  if (this->selfFreeing) {
    free(this->raw);
  }

  {%each fields|fieldsInfo as field %}
    {%if field.hasConstructor %}
  {{ field.name }}.Dispose();
  {{ field.name }}.Clear();
    {%endif%}
  {%endeach%}
}

void {{ cppClassName }}::ConstructFields() {
  {%each fields|fieldsInfo as field %}
    {%if field.hasConstructor %}
  {{ field.name }} = Persistent<Object>::New({{ field.cppClassName }}::New(&this->raw->{{ field.name }})->ToObject());
    {%elsif field.isFunction %}
  // Set the static method call and set the payload for this function to be
  // the current instance
  this->raw->{{ field.name }} = ({{ field.cType }}){{ field.name }}_cppCallback;
  this->raw->{{ fields|payloadFor field.name }} = (void *)this;
    {%endif%}
  {%endeach%}
}

void {{ cppClassName }}::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("{{ jsClassName }}"));

  {%each fields as field%}
    {%if not field.ignore%}
  tpl->InstanceTemplate()->SetAccessor(NanNew<String>("{{ field.jsFunctionName }}"), Get{{ field.cppFunctionName}}, Set{{ field.cppFunctionName}});
    {%endif%}
  {%endeach%}

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("{{ jsClassName }}"), _constructor_template);
}

NAN_METHOD({{ cppClassName }}::New) {
  NanScope();
  {{ cppClassName }}* instance;

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    instance = new {{ cppClassName }}();
  }
  else {
    instance = new {{ cppClassName }}(static_cast<{{ cType }}*>(Handle<External>::Cast(args[0])->Value()));
  }

  instance->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> {{ cppClassName }}::New({{ cType }}* raw) {
  NanEscapableScope();

  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>({{ cppClassName }}::constructor_template)->NewInstance(1, argv));
}

{{ cType }} *{{ cppClassName }}::GetValue() {
  return this->raw;
}

{{ cType }} **{{ cppClassName }}::GetRefValue() {
  return &this->raw;
}

{%partial fieldAccessors .%}

Persistent<Function> {{ cppClassName }}::constructor_template;
