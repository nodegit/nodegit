#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
// generated from class_header.h
#include <nan.h>
#include <string>

extern "C" {
#include <git2.h>
{%each cDependencies as dependency %}
#include <{{ dependency }}>
{%endeach%}
}

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

    {%if cType%}
    {{ cType }} *GetValue();
    {{ cType }} **GetRefValue();
    void ClearValue();

    static Local<v8::Value> New(void *raw, bool selfFreeing);
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
    static void {{ function.cppFunctionName }}_{{ arg.name }}_asyncPromisePolling(uv_async_t* req, int status);
    struct {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton {
      {% each arg.args|argsInfo as cbArg %}
      {{ cbArg.cType }} {{ cbArg.name }};
      {% endeach %}

      uv_async_t req;
      {{ arg.return.type }} result;
      Nan::Persistent<Object> promise;
      bool done;
    };
          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}


  private:


    {%if cType%}
    {{ cppClassName }}({{ cType }} *raw, bool selfFreeing);
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

    {%each fields as field%}
      {%if not field.ignore%}
    static NAN_METHOD({{ field.cppFunctionName }});
      {%endif%}
    {%endeach%}

    {%each functions as function%}
      {%if not function.ignore%}
        {%if function.isAsync%}

    struct {{ function.cppFunctionName }}Baton {
      int error_code;
      const git_error* error;
      {%each function.args as arg%}
        {%if arg.isReturn%}
      {{ arg.cType|replace "**" "*" }} {{ arg.name }};
        {%else%}
      {{ arg.cType }} {{ arg.name }};
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
