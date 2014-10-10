// This is a generated file, modify: generate/templates/class.cc.
#include <nan.h>
#include <string.h>

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

{{ cppClassName }}::{{ cppClassName }}({{ cType }} *raw) {
  this->raw = raw;
}

{{ cppClassName }}::~{{ cppClassName }}() {
  // This is going to cause memory leaks. We'll have to solve that later
  // TODO: Clean up memory better
  //free(this->raw);
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

  {{ cType }} raw = {{ cType|upper }}_INIT;
  {{ cppClassName }}* instance = new {{ cppClassName }}(&raw);
  instance->Wrap(args.This());

  NanReturnValue(args.This());
}

{{ cType }} *{{ cppClassName }}::GetValue() {
  return this->raw;
}

{%partial fieldAccessors .%}

Persistent<Function> {{ cppClassName }}::constructor_template;
