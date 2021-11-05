#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <nan.h>
#include <string>
#include <utility>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <sstream>

#include "async_baton.h"
#include "async_worker.h"
#include "cleanup_handle.h"
#include "context.h"
#include "lock_master.h"
#include "nodegit_wrapper.h"
#include "promise_completion.h"
#include "reference_counter.h"
#include "worker_pool.h"

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
    {{ cppClassName }}(const {{ cppClassName }} &) = delete;
    {{ cppClassName }}({{ cppClassName }} &&) = delete;
    {{ cppClassName }} &operator=(const {{ cppClassName }} &) = delete;
    {{ cppClassName }} &operator=({{ cppClassName }} &&) = delete;

    static void InitializeComponent (v8::Local<v8::Object> target, nodegit::Context *nodegitContext);

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

    static void {{ function.cppFunctionName }}_{{ arg.name }}_cancelAsync(void *baton);
    static void {{ function.cppFunctionName }}_{{ arg.name }}_async(void *baton);
    static void {{ function.cppFunctionName }}_{{ arg.name }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, v8::Local<v8::Value> result);
    class {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton : public nodegit::AsyncBatonWithResult<{{ arg.return.type }}> {
    public:
      {% each arg.args|argsInfo as cbArg %}
      {{ cbArg.cType }} {{ cbArg.name }};
      {% endeach %}


      {{ function.cppFunctionName }}_{{ arg.name|titleCase }}Baton(const {{ arg.return.type }} &defaultResult)
        : nodegit::AsyncBatonWithResult<{{ arg.return.type }}>(defaultResult) {
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
      {% if function.return.isResultOrError %}
      {% elsif function.return.isErrorCode %}
      {% elsif function.return.cType != 'void' %}
        {{ function.return.cType }} result;
      {% endif %}
    };
    class {{ function.cppFunctionName }}Worker : public nodegit::AsyncWorker {
      public:
        {{ function.cppFunctionName }}Worker(
            {{ function.cppFunctionName }}Baton *_baton,
            Nan::Callback *callback,
            std::map<std::string, std::shared_ptr<nodegit::CleanupHandle>> &cleanupHandles
        ) : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:{{ cppClassName }}:{{ function.cppFunctionName }}", cleanupHandles)
          , baton(_baton) {};
        {{ function.cppFunctionName }}Worker(const {{ function.cppFunctionName }}Worker &) = delete;
        {{ function.cppFunctionName }}Worker({{ function.cppFunctionName }}Worker &&) = delete;
        {{ function.cppFunctionName }}Worker &operator=(const {{ function.cppFunctionName }}Worker &) = delete;
        {{ function.cppFunctionName }}Worker &operator=({{ function.cppFunctionName }}Worker &&) = delete;
        ~{{ function.cppFunctionName }}Worker() {};
        void Execute();
        void HandleErrorCallback();
        void HandleOKCallback();
        nodegit::LockMaster AcquireLocks();

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

      {{ function.cppFunctionName }}_globalPayload(const {{ function.cppFunctionName }}_globalPayload &) = delete;
      {{ function.cppFunctionName }}_globalPayload({{ function.cppFunctionName }}_globalPayload &&) = delete;
      {{ function.cppFunctionName }}_globalPayload &operator=(const {{ function.cppFunctionName }}_globalPayload &) = delete;
      {{ function.cppFunctionName }}_globalPayload &operator=({{ function.cppFunctionName }}_globalPayload &&) = delete;

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
