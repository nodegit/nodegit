#include <napi.h>
#include <string.h>

extern "C" {
  #include <git2.h>
  {% each cDependencies as dependency %}
    #include <{{ dependency }}>
  {% endeach %}
}

#include "../include/nodegit.h"
#include "../include/lock_master.h"
#include "../include/functions/copy.h"
#include "../include/{{ filename }}.h"
#include "nodegit_wrapper.cc"

{% each dependencies as dependency %}
  #include "{{ dependency }}"
{% endeach %}

#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

{% if cType %}
  {{ cppClassName }}::~{{ cppClassName }}() {
    // this will cause an error if you have a non-self-freeing object that also needs
    // to save values. Since the object that will eventually free the object has no
    // way of knowing to free these values.
    {% each functions as function %}
      {% if not function.ignore %}
        {% each function.args as arg %}
          {% if arg.saveArg %}

      {{ function.cppFunctionName }}_{{ arg.name }}).Reset();

          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}
  }

  {{ cppClassName }}::{{ cppClassName }}(const Napi::CallbackInfo& info)
    : NodeGitWrapper<{{ cppClassName }}Traits>(info) {
  }

  {{ cppClassName }}Traits::cType* {{ cppClassName }}::DefaultAllocate(Napi::Env env) {
    Napi::Error::New(env, "A new {{ cppClassName }} cannot be instantiated.{% if createFunctionName %} Use {{ jsCreateFunctionName }} instead.{% endif %}").ThrowAsJavaScriptException();
    return nullptr;
  }

  void {{ cppClassName }}::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
    Napi::Env env = target.Env();
    Napi::HandleScope scope(env);

    Napi::Function tpl = Napi::Function::New(env, JSNewFunction, "{{ jsClassName }}");

    Napi::Object proto = tpl.Get("prototype").As<Napi::Object>();
    {% each functions as function %}
      {% if not function.ignore %}
        {% if function.isPrototypeMethod %}
          proto.Set("{{ function.jsFunctionName }}", Napi::Function::New(env, {{ function.cppFunctionName }}_thunk, "{{ function.jsFunctionName }}"));
        {% else %}
          tpl.Set("{{ function.jsFunctionName }}", Napi::Function::New(env, {{ function.cppFunctionName }}_thunk, "{{ function.jsFunctionName }}"));
        {% endif %}
      {% endif %}
    {% endeach %}

    {% each fields as field %}
      {% if not field.ignore %}
        proto.Set("{{ field.jsFunctionName }}", Napi::Function::New(env, {{ field.cppFunctionName }}_thunk, "{{ field.jsFunctionName }}"));
      {% endif %}
    {% endeach %}

    InitializeTemplate(tpl);

    nodegitContext->SaveToPersistent("{{ cppClassName }}::Template", tpl);
    target.Set("{{ jsClassName }}", tpl);
  }

  {% each functions as function %}
    {% if not function.ignore %}
  Napi::Value {{ cppClassName }}::{{ function.cppFunctionName }}_thunk(const Napi::CallbackInfo& info) {
    {% if function.isPrototypeMethod %}
    {{ cppClassName }}* self = static_cast<{{ cppClassName }}*>(nodegit::TrackerWrap::Unwrap(info.This().As<Napi::Object>()));
    return self->{{ function.cppFunctionName }}(info);
    {% else %}
    return {{ cppClassName }}::{{ function.cppFunctionName }}(info);
    {% endif %}
  }
    {% endif %}
  {% endeach %}

  {% each fields as field %}
    {% if not field.ignore %}
  Napi::Value {{ cppClassName }}::{{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info) {
    {{ cppClassName }}* self = static_cast<{{ cppClassName }}*>(nodegit::TrackerWrap::Unwrap(info.This().As<Napi::Object>()));
    return self->{{ field.cppFunctionName }}(info);
  }
    {% endif %}
  {% endeach %}

{% else %}

  void {{ cppClassName }}::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
    Napi::Env env = target.Env();
    Napi::HandleScope scope(env);

    {% if functions|hasFunctionOnRootProto %}
      Napi::Function object = Napi::Function::New(env, {{ functions|getCPPFunctionForRootProto }}_thunk, "{{ jsClassName }}");
    {% else %}
      Napi::Object object = Napi::Object::New(env);
    {% endif %}

    {% each functions as function %}
      {% if not function.ignore %}
        object.Set("{{ function.jsFunctionName }}", Napi::Function::New(env, {{ function.cppFunctionName }}_thunk, "{{ function.jsFunctionName }}"));
      {% endif %}
    {% endeach %}

    target.Set("{{ jsClassName }}", object);
  }

  {% each functions as function %}
    {% if not function.ignore %}
  Napi::Value {{ cppClassName }}::{{ function.cppFunctionName }}_thunk(const Napi::CallbackInfo& info) {
    return {{ cppClassName }}::{{ function.cppFunctionName }}(info);
  }
    {% endif %}
  {% endeach %}

{% endif %}

{% each functions as function %}
  {% if not function.ignore %}
    {% if function.isManual %}
      {{= function.implementation =}}
    {% elsif function.isAsync %}
      {% partial asyncFunction function %}
    {% else %}
      {% partial syncFunction function %}
    {% endif %}
  {% endif %}
{% endeach %}

{% partial fields . %}

{%if cType %}
// force base class template instantiation, to make sure we get all the
// methods, statics, etc.
template class NodeGitWrapper<{{ cppClassName }}Traits>;
{% endif %}
