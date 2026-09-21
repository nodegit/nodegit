// generated from struct_header.h
#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <napi.h>
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
  #include <git2/sys/errors.h>
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
      {{ cppClassName }}(const Napi::CallbackInfo& info);
      static {{ cType }}* DefaultAllocate(Napi::Env env);
      {{ cppClassName }}(const {{ cppClassName }} &) = delete;
      {{ cppClassName }}({{ cppClassName }} &&) = delete;
      {{ cppClassName }} &operator=(const {{ cppClassName }} &) = delete;
      {{ cppClassName }} &operator=({{ cppClassName }} &&) = delete;
      static void InitializeComponent (Napi::Object target, nodegit::Context *nodegitContext);

      {% each fields as field %}
        {% if not field.ignore %}
          static Napi::Value Get{{ field.cppFunctionName }}(const Napi::CallbackInfo& info);
          static void Set{{ field.cppFunctionName }}(const Napi::CallbackInfo& info, const Napi::Value& value);
        {% endif %}
      {% endeach %}

    private:
      ~{{ cppClassName }}();

      void ConstructFields();

      {% each fields as field %}
        {% if not field.ignore %}
          {% if not field.isEnum %}
            {% if field.isLibgitType %}
              Napi::ObjectReference {{ field.name }};
            {% endif %}
          {% endif %}
        {% endif %}
      {% endeach %}
  };
{% endif %}

class Configurable{{ cppClassName }} : public nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits> {
  friend class nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>;

public:
  static v8ConversionResult fromJavascript(nodegit::Context *nodegitContext, Napi::Value input);
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
        static void {{ field.jsFunctionName }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, Napi::Value result);
        {% if field.return.type == 'void' %}
          class {{ field.name|titleCase }}Baton : public nodegit::AsyncBatonWithNoResult {
          public:
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name }};
              {% if arg.cppClassName == "GitIndexerProgress" %}
                git_indexer_progress stats_copy;
              {% endif %}
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
              {% if arg.cppClassName == "GitIndexerProgress" %}
                git_indexer_progress stats_copy;
              {% endif %}
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
  Napi::ObjectReference promiseError;

  {% each fields as field %}
    {% if not field.ignore %}
      {% if not field.isEnum %}
        {% if field.isClassType %}
          {% if field.cppClassName == 'GitOid' %}
            {%-- We do not need to generate anything here --%}
          {% elsif field.cppClassName == 'GitStrarray' %}
            {%-- We do not need to generate anything here --%}
          {% else %}
            Napi::ObjectReference {{ field.jsFunctionName }};
          {% endif %}
        {% elsif field.isCallbackFunction %}
          CallbackWrapper {{ field.jsFunctionName }};
        {% endif %}
      {% endif %}
    {% endif %}
  {% endeach %}

};

#endif
