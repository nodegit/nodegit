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
          this->{{ field.name }}.Reset({{ field.name }}Temp);

        {% elsif field.isCallbackFunction %}

          // Set the static method call and set the payload for this function to be
          // the current instance
          this->raw->{{ field.name }} = NULL;
          this->raw->{{ fields|payloadFor field.name }} = (void *)this;
          this->{{ field.name }} = NULL;
        {% elsif field.payloadFor %}

          Local<Value> {{ field.name }} = Nan::Undefined();
          this->{{ field.name }}.Reset({{ field.name }});
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}
}

void {{ cppClassName }}::InitializeComponent(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("{{ jsClassName }}").ToLocalChecked());

  {% each fields as field %}
    {% if not field.ignore %}
    {% if not field | isPayload %}
      Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("{{ field.jsFunctionName }}").ToLocalChecked(), Get{{ field.cppFunctionName}}, Set{{ field.cppFunctionName}});
    {% endif %}
    {% endif %}
  {% endeach %}

  Local<Function> _constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
  constructor_template.Reset(_constructor_template);
  Nan::Set(target, Nan::New("{{ jsClassName }}").ToLocalChecked(), _constructor_template);
}

NAN_METHOD({{ cppClassName }}::JSNewFunction) {
  {{ cppClassName }}* instance;

  if (info.Length() == 0 || !info[0]->IsExternal()) {
    instance = new {{ cppClassName }}();
  }
  else {
    instance = new {{ cppClassName }}(static_cast<{{ cType }}*>(Local<External>::Cast(info[0])->Value()), Nan::To<bool>(info[1]).FromJust());
  }

  instance->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

Local<v8::Value> {{ cppClassName }}::New(void* raw, bool selfFreeing) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> argv[2] = { Nan::New<External>((void *)raw), Nan::New<Boolean>(selfFreeing) };
  return scope.Escape(Nan::NewInstance(Nan::New({{ cppClassName }}::constructor_template), 2, argv).ToLocalChecked());
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

Nan::Persistent<Function> {{ cppClassName }}::constructor_template;
