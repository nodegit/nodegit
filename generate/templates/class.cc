// This is a generated file, modify: generate/templates/class.cc.ejs.
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

{%if cType%}
{{ cppClassName }}::{{ cppClassName }}({{ cType }} *raw) {
  this->raw = raw;
}

{{ cppClassName }}::~{{ cppClassName }}() {
  {%if freeFunctionName%}{{ freeFunctionName }}(this->raw);{%endif%}
}

void {{ cppClassName }}::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(NanNew<String>("{{ jsClassName }}"));

  {%each functions as function%}
    {%if not function.ignore%}
      {%if function.isPrototypeMethod%}
  NODE_SET_PROTOTYPE_METHOD(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
      {%else%}
  NODE_SET_METHOD(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
      {%endif%}
    {%endif%}
  {%endeach%}

  {%each fields as field%}
    {%if not field.ignore%}
  NODE_SET_PROTOTYPE_METHOD(tpl, "{{ field.jsFunctionName }}", {{ field.cppFunctionName }});
    {%endif%}
  {%endeach%}

  Local<Function> _constructor_template = tpl->GetFunction();
  NanAssignPersistent(constructor_template, _constructor_template);
  target->Set(NanNew<String>("{{ jsClassName }}"), _constructor_template);
}

NAN_METHOD({{ cppClassName }}::New) {
  NanScope();

  if (args.Length() == 0 || !args[0]->IsExternal()) {
    return NanThrowError("{{ cType }} is required.");
  }
  {{ cppClassName }}* object = new {{ cppClassName }}(static_cast<{{ cType }} *>(Handle<External>::Cast(args[0])->Value()));
  object->Wrap(args.This());

  NanReturnValue(args.This());
}

Handle<Value> {{ cppClassName }}::New(void *raw) {
  NanEscapableScope();
  Handle<Value> argv[1] = { NanNew<External>((void *)raw) };
  return NanEscapeScope(NanNew<Function>({{ cppClassName }}::constructor_template)->NewInstance(1, argv));
}

{{ cType }} *{{ cppClassName }}::GetValue() {
  return this->raw;
}
{%else%}
void {{ cppClassName }}::Initialize(Handle<v8::Object> target) {
  NanScope();

  Local<Object> object = NanNew<Object>();

  {%each functions as function%}
    {%if not function.ignore%}
  NODE_SET_METHOD(object, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
    {%endif%}
  {%endeach%}

  target->Set(NanNew<String>("{{ jsClassName }}"), object);
}
{%endif%}

{%each functions as function%}
  {%if not function.ignore%}
    {%each function.args as arg%}
    {%endeach%}
  {%endif%}

  {%if function.isAsync%}
    {%partial asyncFunction .%}
  {%else%}
    {%partial syncFunction .%}
  {%endif%}
{%endeach%}

{%partial fields .%}

{%if cType%}
Persistent<Function> {{ cppClassName }}::constructor_template;
{%endif%}
