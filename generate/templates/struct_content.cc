// This is a generated file, modify: generate/templates/struct_content.cc.
#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

extern "C" {
#include <git2.h>
{%each cDependencies as dependency %}
#include <{{ dependency }}>
{%endeach%}
}

#include "../include/functions/copy.h"
#include "../include/{{ filename }}.h"

{%each dependencies as dependency%}
#include "{{ dependency }}"
{%endeach%}

using namespace v8;
using namespace node;
using namespace std;
// generated from struct_content.cc
{{ cppClassName }}::{{ cppClassName }}() {
  {{ cType }} wrappedValue = {{ cType|upper }}_INIT;
  this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}));
  memcpy(this->raw, &wrappedValue, sizeof({{ cType }}));

  this->ConstructFields();
  this->selfFreeing = true;
}

{{ cppClassName }}::{{ cppClassName }}({{ cType }}* raw, bool selfFreeing) {
  this->raw = raw;
  this->ConstructFields();
  this->selfFreeing = selfFreeing;
}

{{ cppClassName }}::~{{ cppClassName }}() {
  // This is going to cause memory leaks. We'll have to solve that later
  // TODO: Clean up memory better
  if (!this->selfFreeing) {
    free(this->raw);
  }
}

void {{ cppClassName }}::ConstructFields() {
  {%each fields|fieldsInfo as field %}
    {%if not field.ignore %}
      {%if not field.isEnum %}
        {%if field.hasConstructor %}
  Local<Object> {{ field.name }}Temp = {{ field.cppClassName }}::New(&this->raw->{{ field.name }}, false)->ToObject();
  NanAssignPersistent(this->{{ field.name }}, {{ field.name }}Temp);

        {%elsif field.isLibgitType %}
  Local<Object> {{ field.name }}Temp = {{ field.cppClassName }}::New(&this->raw->{{ field.name }}, false)->ToObject();
  NanAssignPersistent(this->{{ field.name }}, {{ field.name }}Temp);

        {%elsif field.isCallbackFunction %}

  // Set the static method call and set the payload for this function to be
  // the current instance
  this->raw->{{ field.name }} = ({{ field.cType }}){{ field.name }}_cppCallback;
  this->raw->{{ fields|payloadFor field.name }} = (void *)this;
  this->{{ field.name }} = new NanCallback();
        {%elsif field.payloadFor %}

  Local<Value> {{ field.name }} = NanUndefined();
  NanAssignPersistent(this->{{ field.name }}, {{ field.name }});
        {%endif%}
      {%endif%}
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
    instance = new {{ cppClassName }}(static_cast<{{ cType }}*>(Handle<External>::Cast(args[0])->Value()), args[1]->BooleanValue());
  }

  instance->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> {{ cppClassName }}::New({{ cType }}* raw, bool selfFreeing) {
  NanEscapableScope();

  Handle<Value> argv[2] = { NanNew<External>((void *)raw), NanNew<Boolean>(selfFreeing) };
  return NanEscapeScope(NanNew<Function>({{ cppClassName }}::constructor_template)->NewInstance(2, argv));
}

{{ cType }} *{{ cppClassName }}::GetValue() {
  return this->raw;
}

{{ cType }} **{{ cppClassName }}::GetRefValue() {
  return &this->raw;
}

{%partial fieldAccessors .%}

Persistent<Function> {{ cppClassName }}::constructor_template;
