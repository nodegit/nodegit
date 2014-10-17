#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
// generated from class_header.h
#include <nan.h>
#include <string>

extern "C" {
#include <git2.h>
}

{%each dependencies as dependency%}
#include "{{ dependency }}"
{%endeach%}

{%if forwardDeclare%}
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

class {{ cppClassName }} : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    {%if cType%}
    {{ cType }} *GetValue();
    {{ cType }} **GetRefValue();

    static Handle<Value> New(void *raw, bool selfFreeing);
    {%endif%}
    bool selfFreeing;

  private:
    {%if cType%}
    {{ cppClassName }}({{ cType }} *raw, bool selfFreeing);
    ~{{ cppClassName }}();
    {%endif%}

    static NAN_METHOD(New);

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
        {%endif%}
      {%endeach%}
    };
    class {{ function.cppFunctionName }}Worker : public NanAsyncWorker {
      public:
        {{ function.cppFunctionName }}Worker(
            {{ function.cppFunctionName }}Baton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
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

    {%if cType%}
    {{ cType }} *raw;
    {%endif%}
};

#endif
