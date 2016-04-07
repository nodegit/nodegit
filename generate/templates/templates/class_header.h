#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <nan.h>
#include <string>
#include <queue>
#include <utility>

#include "async_baton.h"
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

class {{ cppClassName }} : public Nan::ObjectWrap {
  public:

    static Nan::Persistent<Function> constructor_template;
    static void InitializeComponent (Local<v8::Object> target);
    // diagnostic count of self-freeing object instances
    static int SelfFreeingInstanceCount;
    // diagnostic count of constructed non-self-freeing object instances
    static int NonSelfFreeingConstructedCount;

    {%if cType%}
    {{ cType }} *GetValue();
    void ClearValue();

    static Local<v8::Value> New(const {{ cType }} *raw, bool selfFreeing, Local<v8::Object> owner = Local<v8::Object>());
    {%endif%}
    bool selfFreeing;

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

    static void {{ function.cppFunctionName }}_{{ arg.name }}_async(uv_async_t* req, int status);
    static void {{ function.cppFunctionName }}_{{ arg.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result);
    struct {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton : AsyncBaton {
      {% each arg.args|argsInfo as cbArg %}
      {{ cbArg.cType }} {{ cbArg.name }};
      {% endeach %}

      {{ arg.return.type }} result;
    };
          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}


  private:
    // owner of the object, in the memory management sense. only populated
    // when using ownedByThis, and the type doesn't have a dupFunction
    // CopyablePersistentTraits are used to get the reset-on-destruct behavior.
    {%if not dupFunction %}
    Nan::Persistent<Object, Nan::CopyablePersistentTraits<Object> > owner;
    {%endif%}

    {%if cType%}
    {{ cppClassName }}({{ cType }} *raw, bool selfFreeing, Local<v8::Object> owner = Local<v8::Object>());
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

    static NAN_METHOD(JSNewFunction);
    static NAN_METHOD(GetSelfFreeingInstanceCount);
    static NAN_METHOD(GetNonSelfFreeingConstructedCount);

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

    {%if cType%}
    {{ cType }} *raw;
    {%endif%}
};

#endif
