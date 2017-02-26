#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <nan.h>
#include <string>
#include <queue>
#include <utility>

#include "async_baton.h"
#include "callback_wrapper.h"
#include "nodegit_wrapper.h"

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

class {{ cppClassName }} : public NodeGitWrapper<{{ cppClassName }}Traits> {
    // grant full access to base class
    friend class NodeGitWrapper<{{ cppClassName }}Traits>;
  public:
    {{ cppClassName }}({{ cType }}* raw, bool selfFreeing, v8::Local<v8::Object> owner = v8::Local<v8::Object>());
    static void InitializeComponent (v8::Local<v8::Object> target);

    {% each fields as field %}
      {% if not field.ignore %}
        {% if field.isCallbackFunction %}
          static {{ field.return.type }} {{ field.name }}_cppCallback (
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name}}
              {% if not arg.lastArg %}
                ,
              {% endif %}
            {% endeach %}
          );

          static void {{ field.name }}_async(void *baton);
          static void {{ field.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result);
          struct {{ field.name|titleCase }}Baton : public AsyncBatonWithResult<{{ field.return.type }}> {
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name}};
            {% endeach %}

            {{ field.name|titleCase }}Baton(const {{ field.return.type }} &defaultResult)
              : AsyncBatonWithResult<{{ field.return.type }}>(defaultResult) {
              }
          };
          static {{ cppClassName }} * {{ field.name }}_getInstanceFromBaton (
            {{ field.name|titleCase }}Baton *baton);
        {% endif %}
      {% endif %}
    {% endeach %}

  private:
    {{ cppClassName }}();
    ~{{ cppClassName }}();

    void ConstructFields();

    {% each fields as field %}
      {% if not field.ignore %}
        {% if not field.isEnum %}
          {% if field.isLibgitType %}
            Nan::Persistent<Object> {{ field.name }};
          {% elsif field.isCallbackFunction %}
            CallbackWrapper {{ field.name }};
          {% elsif field.payloadFor %}
            Nan::Persistent<Value> {{ field.name }};
          {% endif %}
        {% endif %}

        static NAN_GETTER(Get{{ field.cppFunctionName }});
        static NAN_SETTER(Set{{ field.cppFunctionName }});

      {% endif %}
    {% endeach %}
};

#endif
