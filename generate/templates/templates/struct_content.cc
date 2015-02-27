// This is a generated file, modify: generate/templates/struct_content.cc.
#include <nan.h>
#include <string.h>
#include <chrono>
#include <thread>

extern "C" {
  #include <git2.h>
  {% each cDependencies as dependency %}
    #include <{{ dependency }}>
  {% endeach %}
}

#include <iostream>
#include "../include/functions/copy.h"
#include "../include/{{ filename }}.h"

{% each dependencies as dependency %}
  #include "{{ dependency }}"
{% endeach %}

using namespace v8;
using namespace node;
using namespace std;


// generated from struct_content.cc
{{ cppClassName }}::{{ cppClassName }}() {
  {% if ignoreInit == true %}
  {{ cType }}* wrappedValue = new {{ cType }};
  {% else %}
  {{ cType }} wrappedValue = {{ cType|upper }}_INIT;
  {% endif %}
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
  {% each fields|fieldsInfo as field %}
    {% if not field.ignore %}
      {% if not field.isEnum %}
        {% if field.isCallbackFunction %}
  if (this->{{ field.name }} != NULL) {
    delete this->{{ field.name }};
    this->raw->{{ fields|payloadFor field.name }} = NULL;
  }
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}

  if (this->selfFreeing) {
    free(this->raw);
  }
}

void {{ cppClassName }}::ConstructFields() {
  {% each fields|fieldsInfo as field %}
    {% if not field.ignore %}
      {% if not field.isEnum %}
        {% if field.hasConstructor |or field.isLibgitType %}
          Local<Object> {{ field.name }}Temp = {{ field.cppClassName }}::New(
            &this->raw->{{ field.name }},
            false
          )->ToObject();
          NanAssignPersistent(this->{{ field.name }}, {{ field.name }}Temp);

        {% elsif field.isCallbackFunction %}

          // Set the static method call and set the payload for this function to be
          // the current instance
          this->raw->{{ field.name }} = NULL;
          this->raw->{{ fields|payloadFor field.name }} = (void *)this;
          this->{{ field.name }} = NULL;
        {% elsif field.payloadFor %}

          Local<Value> {{ field.name }} = NanUndefined();
          NanAssignPersistent(this->{{ field.name }}, {{ field.name }});
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}
}

void {{ cppClassName }}::InitializeComponent(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("{{ jsClassName }}"));

  {% each fields as field %}
    {% if not field.ignore %}
    {% if not field | isPayload %}
      tpl->InstanceTemplate()->SetAccessor(NanNew<String>("{{ field.jsFunctionName }}"), Get{{ field.cppFunctionName}}, Set{{ field.cppFunctionName}});
    {% endif %}
    {% endif %}
  {% endeach %}

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("{{ jsClassName }}"), _constructor_template);
}

NAN_METHOD({{ cppClassName }}::JSNewFunction) {
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

Handle<v8::Value> {{ cppClassName }}::New(void* raw, bool selfFreeing) {
  NanEscapableScope();

  Handle<v8::Value> argv[2] = { NanNew<External>((void *)raw), NanNew<Boolean>(selfFreeing) };
  return NanEscapeScope(NanNew<Function>({{ cppClassName }}::constructor_template)->NewInstance(2, argv));
}

{{ cType }} *{{ cppClassName }}::GetValue() {
  return this->raw;
}

{{ cType }} **{{ cppClassName }}::GetRefValue() {
  return this->raw == NULL ? NULL : &this->raw;
}

void {{ cppClassName }}::ClearValue() {
  this->raw = NULL;
}

{% partial fieldAccessors . %}

Persistent<Function> {{ cppClassName }}::constructor_template;
