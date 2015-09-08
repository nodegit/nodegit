#ifndef {{ cppClassName|upper }}_H
#define {{ cppClassName|upper }}_H
// generated from struct_header.h
#include <nan.h>
#include <string>

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

class {{ cppClassName }} : public Nan::ObjectWrap {
  public:
    {{ cppClassName }}({{ cType }}* raw, bool selfFreeing);
    static Nan::Persistent<Function> constructor_template;
    static void InitializeComponent (Local<v8::Object> target);

    {{ cType }} *GetValue();
    {{ cType }} **GetRefValue();
    void ClearValue();

    static Local<v8::Value> New(void *raw, bool selfFreeing);

    bool selfFreeing;

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

          static void {{ field.name }}_async(uv_async_t* req, int status);
          static void {{ field.name }}_asyncPromisePolling(uv_async_t* req, int status);
          struct {{ field.name|titleCase }}Baton {
            {% each field.args|argsInfo as arg %}
              {{ arg.cType }} {{ arg.name}};
            {% endeach %}

            uv_async_t req;
            {{ field.return.type }} result;
            Nan::Persistent<Object> promise;
            bool done;
          };
        {% endif %}
      {% endif %}
    {% endeach %}

  private:
    {{ cppClassName }}();
    ~{{ cppClassName }}();

    void ConstructFields();

    static NAN_METHOD(JSNewFunction);

    {% each fields as field %}
      {% if not field.ignore %}
        {% if not field.isEnum %}
          {% if field.isLibgitType %}
            Nan::Persistent<Object> {{ field.name }};
          {% elsif field.isCallbackFunction %}
            Nan::Callback* {{ field.name }};
          {% elsif field.payloadFor %}
            Nan::Persistent<Value> {{ field.name }};
          {% endif %}
        {% endif %}

        static NAN_GETTER(Get{{ field.cppFunctionName }});
        static NAN_SETTER(Set{{ field.cppFunctionName }});

      {% endif %}
    {% endeach %}

    {{ cType }} *raw;
};

#endif
