#include <nan.h>
#include <string.h>

extern "C" {
  #include <git2.h>
  {% each cDependencies as dependency %}
    #include <{{ dependency }}>
  {% endeach %}
}

#include "../include/lock_master.h"
#include "../include/functions/copy.h"
#include "../include/{{ filename }}.h"
#include "../include/functions/sleep_for_ms.h"

{% each dependencies as dependency %}
  #include "{{ dependency }}"
{% endeach %}

#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

{% if cType %}
  {{ cppClassName }}::{{ cppClassName }}({{ cType }} *raw, bool selfFreeing, Local<v8::Object> owner) {
    if (!owner.IsEmpty()) {
      // if we have an owner, there are two options - either we duplicate the raw object
      // (so we own the duplicate, and can self-free it)
      // or we keep a handle on the owner so it doesn't get garbage collected
      // while this wrapper is accessible
      {% if dupFunction %}
        {% if shouldAlloc %}
      this->raw = ({{ cType }} *)malloc(sizeof({{ cType }}));
      {{ dupFunction }}(this->raw, raw);
        {% else %}
      {{ dupFunction }}(&this->raw, raw);
        {% endif %}
      selfFreeing = true;
      {% else %}
      this->owner.Reset(owner);
      this->raw = raw;
      {% endif %}
    } else {
      this->raw = raw;
    }
    this->selfFreeing = selfFreeing;

    if (selfFreeing) {
      SelfFreeingInstanceCount++;
    } else {
      NonSelfFreeingConstructedCount++;
    }

  }

  {{ cppClassName }}::~{{ cppClassName }}() {
    {% if freeFunctionName %}
      if (this->selfFreeing) {
        {{ freeFunctionName }}(this->raw);
        SelfFreeingInstanceCount--;

        this->raw = NULL;
      }
    {% endif %}

    // this will cause an error if you have a non-self-freeing object that also needs
    // to save values. Since the object that will eventually free the object has no
    // way of knowing to free these values.
    {% each functions as function %}
      {% if not function.ignore %}
        {% each function.args as arg %}
          {% if arg.saveArg %}

      {{ function.cppFunctionName }}_{{ arg.name }}).Reset();

          {% endif %}
        {% endeach %}
      {% endif %}
    {% endeach %}
  }

  void {{ cppClassName }}::InitializeComponent(Local<v8::Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction);

    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(Nan::New("{{ jsClassName }}").ToLocalChecked());

    {% each functions as function %}
      {% if not function.ignore %}
        {% if function.isPrototypeMethod %}
          Nan::SetPrototypeMethod(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
        {% else %}
          Nan::SetMethod(tpl, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
        {% endif %}
      {% endif %}
    {% endeach %}

    {% each fields as field %}
      {% if not field.ignore %}
        Nan::SetPrototypeMethod(tpl, "{{ field.jsFunctionName }}", {{ field.cppFunctionName }});
      {% endif %}
    {% endeach %}

    Nan::SetMethod(tpl, "getSelfFreeingInstanceCount", GetSelfFreeingInstanceCount);
    Nan::SetMethod(tpl, "getNonSelfFreeingConstructedCount", GetNonSelfFreeingConstructedCount);

    Local<Function> _constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
    constructor_template.Reset(_constructor_template);
    Nan::Set(target, Nan::New("{{ jsClassName }}").ToLocalChecked(), _constructor_template);
  }

  NAN_METHOD({{ cppClassName }}::JSNewFunction) {

    if (info.Length() == 0 || !info[0]->IsExternal()) {
      {% if createFunctionName %}
        return Nan::ThrowError("A new {{ cppClassName }} cannot be instantiated. Use {{ jsCreateFunctionName }} instead.");
      {% else %}
        return Nan::ThrowError("A new {{ cppClassName }} cannot be instantiated.");
      {% endif %}
    }

    {{ cppClassName }}* object = new {{ cppClassName }}(static_cast<{{ cType }} *>(
      Local<External>::Cast(info[0])->Value()),
      Nan::To<bool>(info[1]).FromJust(),
      info.Length() >= 3 && !info[2].IsEmpty() && info[2]->IsObject() ? info[2]->ToObject() : Local<v8::Object>()
    );
    object->Wrap(info.This());

    info.GetReturnValue().Set(info.This());
  }

  Local<v8::Value> {{ cppClassName }}::New(const {{ cType }} *raw, bool selfFreeing, Local<v8::Object> owner) {
    Nan::EscapableHandleScope scope;
    Local<v8::Value> argv[3] = { Nan::New<External>((void *)raw), Nan::New(selfFreeing), owner };
    return scope.Escape(
      Nan::NewInstance(
        Nan::New({{ cppClassName }}::constructor_template),
        owner.IsEmpty() ? 2 : 3, // passing an empty handle as part of the arguments causes a crash
        argv
      ).ToLocalChecked());
  }

  NAN_METHOD({{ cppClassName }}::GetSelfFreeingInstanceCount) {
    info.GetReturnValue().Set(SelfFreeingInstanceCount);
  }

  NAN_METHOD({{ cppClassName }}::GetNonSelfFreeingConstructedCount) {
    info.GetReturnValue().Set(NonSelfFreeingConstructedCount);
  }

  {{ cType }} *{{ cppClassName }}::GetValue() {
    return this->raw;
  }

  void {{ cppClassName }}::ClearValue() {
    this->raw = NULL;
  }

{% else %}

  void {{ cppClassName }}::InitializeComponent(Local<v8::Object> target) {
    Nan::HandleScope scope;

    Local<Object> object = Nan::New<Object>();

    {% each functions as function %}
      {% if not function.ignore %}
        Nan::SetMethod(object, "{{ function.jsFunctionName }}", {{ function.cppFunctionName }});
      {% endif %}
    {% endeach %}

    Nan::Set(target, Nan::New<String>("{{ jsClassName }}").ToLocalChecked(), object);
  }

{% endif %}

{% each functions as function %}
  {% if not function.ignore %}
    {% if function.isManual %}
      {{= function.implementation =}}
    {% elsif function.isAsync %}
      {% partial asyncFunction function %}
    {% else %}
      {% partial syncFunction function %}
    {% endif %}
  {% endif %}
{% endeach %}

{% partial fields . %}

{% if not cTypeIsUndefined %}
  Nan::Persistent<Function> {{ cppClassName }}::constructor_template;
{% endif %}

int {{ cppClassName }}::SelfFreeingInstanceCount;
int {{ cppClassName }}::NonSelfFreeingConstructedCount;
