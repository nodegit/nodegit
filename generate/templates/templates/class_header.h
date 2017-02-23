#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <nan.h>
#include <string>
#include <queue>
#include <utility>

#include "async_baton.h"
#include "nodegit_wrapper.h"
#include "promise_completion.h"

extern "C" {
#include <git2.h>
{%each cDependencies as dependency %}
#include <{{ dependency }}>
{%endeach%}
}

#include "../include/typedefs.h"

{%each dependencies as dependency%}
#include "{{ dependency }}"
{%endeach%}

{%if needsForwardDeclaration %}
// Forward declaration.
struct {{ cType }} {
  {%each fields as field%}
    {%if not field.ignore%}
      {{ field.structType|or field.cType }} {{ field.structName|or field.name }};
    {%endif%}
  {%endeach%}
};
{%endif%}

using namespace node;
using namespace v8;

{%if cType %}
{%partial traits .%}
{%endif%}

class {{ cppClassName }} : public
{%if cType %}
  NodeGitWrapper<{{ cppClassName }}Traits>
{%else%}
  Nan::ObjectWrap
{%endif%}
{
  {%if cType %}
    // grant full access to base class
    friend class NodeGitWrapper<{{ cppClassName }}Traits>;
  {%endif %}
  public:
    static void InitializeComponent (v8::Local<v8::Object> target);

    {% each functions as function %}
      {% if not function.ignore %}
        {%each function.args as arg %}
          {%if arg.isCallbackFunction %}
    static {{ arg.return.type }} {{ function.cppFunctionName }}_{{ arg.name }}_cppCallback (
            {% each arg.args|argsInfo as cbArg %}
      {{ cbArg.cType }} {{ cbArg.name }}
              {% if not cbArg.lastArg %}
      ,
              {% endif %}
            {% endeach %}
    );

    static void {{ function.cppFunctionName }}_{{ arg.name }}_async(void *baton);
    static void {{ function.cppFunctionName }}_{{ arg.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result);
    struct {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton : public AsyncBatonWithResult<{{ arg.return.type }}> {
      {% each arg.args|argsInfo as cbArg %}
      {{ cbArg.cType }} {{ cbArg.name }};
      {% endeach %}

      {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton(const {{ arg.return.type }} &defaultResult)
        : AsyncBatonWithResult<{{ arg.return.type }}>(defaultResult) {
        }
    };
          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}


  private:
    {%if cType%}
    {{ cppClassName }}()
      : NodeGitWrapper<{{ cppClassName }}Traits>(
        {% if createFunctionName %}
          "A new {{ cppClassName }} cannot be instantiated. Use {{ jsCreateFunctionName }} instead."
        {% else %}
          "A new {{ cppClassName }} cannot be instantiated."
        {% endif %}
      )
    {}
    {{ cppClassName }}({{ cType }} *raw, bool selfFreeing, v8::Local<v8::Object> owner = v8::Local<v8::Object>())
      : NodeGitWrapper<{{ cppClassName }}Traits>(raw, selfFreeing, owner)
    {}
    ~{{ cppClassName }}();
    {%endif%}

    {% each functions as function %}
      {% if not function.ignore %}
        {% each function.args as arg %}
          {% if arg.saveArg %}
    Nan::Persistent<Object> {{ function.cppFunctionName }}_{{ arg.name }};
          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}

    {%each fields as field%}
      {%if not field.ignore%}
    static NAN_METHOD({{ field.cppFunctionName }});
      {%endif%}
    {%endeach%}

    {%each functions as function%}
      {%if not function.ignore %}
        {%if function.isAsync %}

    struct {{ function.cppFunctionName }}Baton {
      int error_code;
      const git_error* error;
      {%each function.args as arg%}
        {%if arg.isReturn%}
      {{= arg.cType|replace "**" "*" =}} {{ arg.name }};
        {%else%}
      {{= arg.cType =}} {{ arg.name }};
          {%if arg | isOid %}
      bool {{ arg.name }}NeedsFree;
          {%endif%}
        {%endif%}
      {%endeach%}
    };
    class {{ function.cppFunctionName }}Worker : public Nan::AsyncWorker {
      public:
        {{ function.cppFunctionName }}Worker(
            {{ function.cppFunctionName }}Baton *_baton,
            Nan::Callback *callback
        ) : Nan::AsyncWorker(callback)
          , baton(_baton) {};
        ~{{ function.cppFunctionName }}Worker() {};
        void Execute();
        void HandleOKCallback();

      private:
        {{ function.cppFunctionName }}Baton *baton;
    };
        {%endif%}

    static NAN_METHOD({{ function.cppFunctionName }});
      {%endif%}
    {%endeach%}

    {%each functions as function%}
      {%each function.args as arg %}
        {% if arg.globalPayload %}

    struct {{ function.cppFunctionName }}_globalPayload {
          {%each function.args as arg %}
            {%if arg.isCallbackFunction %}
      Nan::Callback * {{ arg.name }};
            {%endif%}
          {%endeach%}

      {{ function.cppFunctionName }}_globalPayload() {
          {%each function.args as arg %}
            {%if arg.isCallbackFunction %}
        {{ arg.name }} = NULL;
            {%endif%}
          {%endeach%}
      }

      ~{{ function.cppFunctionName }}_globalPayload() {
          {%each function.args as arg %}
            {%if arg.isCallbackFunction %}
        if ({{ arg.name }} != NULL) {
          delete {{ arg.name }};
        }
            {%endif%}
          {%endeach%}
      }
    };
        {%endif%}
      {%endeach%}
    {%endeach%}
};

#endif
