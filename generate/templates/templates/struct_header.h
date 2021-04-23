// generated from struct_header.h
#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <nan.h>
#include <string>
#include <utility>

#include "async_baton.h"
#include "async_worker.h"
#include "callback_wrapper.h"
#include "context.h"
#include "reference_counter.h"
#include "nodegit_wrapper.h"
#include "configurable_class_wrapper.h"
#include "v8_helpers.h"

extern "C" {
  #include <git2.h>
  {% each cDependencies as dependency %}
    #include <{{ dependency }}>
  {% endeach %}
}

{% each dependencies as dependency %}
  #include "{{ dependency }}"
{% endeach %}

using namespace node;
using namespace v8;

{%partial traits .%}
{% if isExtendedStruct %}
struct {{ cType }}_extended {
  {{ cType }} raw;
  void* payload;
};
{% endif %}
{% if isReturnable %}
  class {{ cppClassName }} : public NodeGitWrapper<{{ cppClassName }}Traits> {
      // grant full access to base class
      friend class NodeGitWrapper<{{ cppClassName }}Traits>;

    public:
      {{ cppClassName }}({{ cType }}* raw, bool selfFreeing, v8::Local<v8::Object> owner = v8::Local<v8::Object>());
      {{ cppClassName }}(const {{ cppClassName }} &) = delete;
      {{ cppClassName }}({{ cppClassName }} &&) = delete;
      {{ cppClassName }} &operator=(const {{ cppClassName }} &) = delete;
      {{ cppClassName }} &operator=({{ cppClassName }} &&) = delete;
      static void InitializeComponent (v8::Local<v8::Object> target, nodegit::Context *nodegitContext);

    private:
      {{ cppClassName }}();
      ~{{ cppClassName }}();

      void ConstructFields();

      {% each fields as field %}
        {% if not field.ignore %}
          {% if not field.isEnum %}
            {% if field.isLibgitType %}
              Nan::Global<Value> {{ field.name }};
            {% endif %}
          {% endif %}

          static NAN_GETTER(Get{{ field.cppFunctionName }});
          static NAN_SETTER(Set{{ field.cppFunctionName }});

        {% endif %}
      {% endeach %}
  };
{% endif %}

class Configurable{{ cppClassName }} : public nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits> {
  friend class nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>;

public:
  static v8ConversionResult fromJavascript(nodegit::Context *nodegitContext, v8::Local<v8::Value> input);
  ~Configurable{{ cppClassName }}();

  Configurable{{ cppClassName }}(const Configurable{{ cppClassName }} &) = delete;
  Configurable{{ cppClassName }}(Configurable{{ cppClassName }} &&) = delete;
  Configurable{{ cppClassName }} &operator=(const Configurable{{ cppClassName }} &) = delete;
  Configurable{{ cppClassName }} &operator=(Configurable{{ cppClassName }} &&) = delete;

  {% each fields as field %}
    {% if not field.ignore %}
      {% if field.isCallbackFunction %}
        static {{ field.return.type }} {{ field.jsFunctionName }}_cppCallback (
          {% each field.args|argsInfo as arg %}
            {{ arg.cType }} {{ arg.name}}
            {% if not arg.lastArg %}
              ,
            {% endif %}
          {% endeach %}
        );

        static void {{ field.jsFunctionName }}_cancelAsync(void *baton);
        static void {{ field.jsFunctionName }}_async(void *baton);
        static void {{ field.jsFunctionName }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, v8::Local<v8::Value> result);
        {% if field.return.type == 'void' %}
          class {{ field.name|titleCase }}Baton : public nodegit::AsyncBatonWithNoResult {
          public:
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name }};
            {% endeach %}

            {{ field.name|titleCase }}Baton()
              : nodegit::AsyncBatonWithNoResult() {
              }
          };
        {% else %}
          class {{ field.name|titleCase }}Baton : public nodegit::AsyncBatonWithResult<{{ field.return.type }}> {
          public:
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name }};
            {% endeach %}

            {{ field.name|titleCase }}Baton(const {{ field.return.type }} &defaultResult)
              : nodegit::AsyncBatonWithResult<{{ field.return.type }}>(defaultResult) {
              }
          };
        {% endif %}
        static Configurable{{ cppClassName }} * {{ field.jsFunctionName }}_getInstanceFromBaton (
          {{ field.name|titleCase }}Baton *baton);
      {% endif %}
    {% endif %}
  {% endeach %}

private:
  Configurable{{ cppClassName }}(nodegit::Context *nodegitContext);
  Configurable{{ cppClassName }}() = delete;
  Nan::Global<Value> promiseError;

  {% each fields as field %}
    {% if not field.ignore %}
      {% if not field.isEnum %}
        {% if field.isClassType %}
          {% if field.cppClassName == 'GitOid' %}
            {%-- We do not need to generate anything here --%}
          {% elsif field.cppClassName == 'GitStrarray' %}
            {%-- We do not need to generate anything here --%}
          {% else %}
            Nan::Global<Object> {{ field.jsFunctionName }};
          {% endif %}
        {% elsif field.isCallbackFunction %}
          CallbackWrapper {{ field.jsFunctionName }};
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}

};

#endif
