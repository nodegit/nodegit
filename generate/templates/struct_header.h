#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H

#include <nan.h>
#include <string>

extern "C" {
#include <git2.h>
}

{%each dependencies as dependency%}
#include "{{ dependency }}"
{%endeach%}

using namespace node;
using namespace v8;

class {{ cppClassName }} : public ObjectWrap {
  public:
    {{ cppClassName }}({{ cType }}* raw);
    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    {{ cType }} *GetValue();
    {{ cType }} **GetRefValue();

    static Handle<Value> New({{ cType }} *raw);

    bool selfFreeing;

    {%each fields as field %}
      {%if not field.ignore %}
        {%if field.isFunction %}
    static {{ field.returnType }} {{ field.name }}_cppCallback (
          {%each field.args|argsInfo as arg%}
      {{ arg.cType }} {{ arg.name}}{%if not arg.lastArg %},{%endif%}
          {%endeach%}
      );
        {%endif%}
      {%endif%}
    {%endeach%}

  private:
    {{ cppClassName }}();
    ~{{ cppClassName }}();

    void ConstructFields();

    static NAN_METHOD(New);

    {%each fields as field%}
      {%if not field.ignore%}
        {%if field.hasConstructor | or field.payloadFor %}
    Persistent<Object> {{ field.name }};
        {%elsif field.isFunction %}
    Persistent<Function> {{ field.name }};
        {%endif%}
    static NAN_GETTER(Get{{ field.cppFunctionName }});
    static NAN_SETTER(Set{{ field.cppFunctionName }});
      {%endif%}
    {%endeach%}

    {{ cType }} *raw;
};

#endif
