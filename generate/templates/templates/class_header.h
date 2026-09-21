#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
#include <napi.h>
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
#include "v8_helpers.h"

extern "C" {
#include <git2.h>
#include <git2/sys/errors.h>
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


{%if cType %}
{%partial traits .%}
{%endif%}

class {{ cppClassName }} : public
{%if cType %}
  NodeGitWrapper<{{ cppClassName }}Traits>
{%else%}
  Napi::ObjectWrap<{{ cppClassName }}>
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

    static void InitializeComponent (Napi::Object target, nodegit::Context *nodegitContext);

    {%if cType %}
    static {{ cType }}* DefaultAllocate(Napi::Env env);
    {%endif %}

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
    static void {{ function.cppFunctionName }}_{{ arg.name }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, Napi::Value result);
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
    {{ cppClassName }}(const Napi::CallbackInfo& info);
    ~{{ cppClassName }}();
    {%endif%}

    {%each fields as field%}
      {%if not field.ignore%}
    static Napi::Value {{ field.cppFunctionName }}(const Napi::CallbackInfo& info);
    static Napi::Value {{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info);
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
            Napi::FunctionReference *callback,
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

    static Napi::Value {{ function.cppFunctionName }}(const Napi::CallbackInfo& info);
    static Napi::Value {{ function.cppFunctionName }}_thunk(const Napi::CallbackInfo& info);
      {%endif%}
    {%endeach%}

    {%each functions as function%}
      {%each function.args as arg %}
        {% if arg.globalPayload %}

    struct {{ function.cppFunctionName }}_globalPayload {
          {%each function.args as arg %}
            {%if arg.isCallbackFunction %}
      Napi::FunctionReference * {{ arg.name }};
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
