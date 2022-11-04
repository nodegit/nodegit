#include <nan.h>
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

  void {{ cppClassName }}::InitializeComponent(v8::Local<v8::Object> target, nodegit::Context *nodegitContext) {
    Nan::HandleScope scope;

    v8::Local<v8::External> nodegitExternal = Nan::New<v8::External>(nodegitContext);
    v8::Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction, nodegitExternal);

    tpl->InstanceTemplate()->SetInternalFieldCount(2);
    tpl->SetClassName(Nan::New("{{ jsClassName }}").ToLocalChecked());

    {% each functions as function %}
      {% if not function.ignore %}
        {% if function.isPrototypeMethod %}
          Nan::SetPrototypeMethod(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }}, nodegitExternal);
        {% else %}
          Nan::SetMethod(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }}, nodegitExternal);
        {% endif %}
      {% endif %}
    {% endeach %}

    {% each fields as field %}
      {% if not field.ignore %}
        Nan::SetPrototypeMethod(tpl, "{{ field.jsFunctionName }}", {{ field.cppFunctionName }}, nodegitExternal);
      {% endif %}
    {% endeach %}

    InitializeTemplate(tpl);

    v8::Local<Function> constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
    nodegitContext->SaveToPersistent("{{ cppClassName }}::Template", constructor_template);
    Nan::Set(target, Nan::New("{{ jsClassName }}").ToLocalChecked(), constructor_template);
  }

{% else %}

  void {{ cppClassName }}::InitializeComponent(v8::Local<v8::Object> target, nodegit::Context *nodegitContext) {
    Nan::HandleScope scope;
    Local<External> nodegitExternal = Nan::New<External>(nodegitContext);

    {% if functions|hasFunctionOnRootProto %}
      v8::Local<FunctionTemplate> object = Nan::New<FunctionTemplate>({{ functions|getCPPFunctionForRootProto }}, nodegitExternal);
    {% else %}
      v8::Local<Object> object = Nan::New<Object>();
    {% endif %}

    {% each functions as function %}
      {% if not function.ignore %}
        Nan::SetMethod(object, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }}, nodegitExternal);
      {% endif %}
    {% endeach %}

    Nan::Set(
      target,
      Nan::New("{{ jsClassName }}").ToLocalChecked(),
      {% if functions|hasFunctionOnRootProto %}
        Nan::GetFunction(object).ToLocalChecked()
      {% else %}
        object
      {% endif %}
    );
  }

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
