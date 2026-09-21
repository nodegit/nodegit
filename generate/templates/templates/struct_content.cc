// generated from struct_content.cc
#include <napi.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // win32

extern "C" {
  #include <git2.h>
  {% each cDependencies as dependency %}
    #include <{{ dependency }}>
  {% endeach %}
}

#include <iostream>
#include "../include/nodegit.h"
#include "../include/lock_master.h"
#include "../include/functions/copy.h"
#include "../include/{{ filename }}.h"
#include "nodegit_wrapper.cc"

{% each dependencies as dependency %}
  #include "{{ dependency }}"
{% endeach %}

using namespace v8;
using namespace node;
using namespace std;

{% if isReturnable %}
  {{ cppClassName }}::{{ cppClassName }}(const Napi::CallbackInfo& info)
    : NodeGitWrapper<{{ cppClassName }}Traits>(info) {
    this->ConstructFields();
  }

  {{ cppClassName }}Traits::cType* {{ cppClassName }}::DefaultAllocate(Napi::Env env) {
    {% if ignoreInit == true %}
    return new {{ cType }};
    {% else %}
      {% if isExtendedStruct %}
        {{ cType }}_extended wrappedValue = {{ cType|upper }}_INIT;
        {{ cType }}* raw = ({{ cType }}*) malloc(sizeof({{ cType }}_extended));
        memcpy(raw, &wrappedValue, sizeof({{ cType }}_extended));
        return raw;
      {% else %}
        {{ cType }} wrappedValue = {{ cType|upper }}_INIT;
        {{ cType }}* raw = ({{ cType }}*) malloc(sizeof({{ cType }}));
        memcpy(raw, &wrappedValue, sizeof({{ cType }}));
        return raw;
      {% endif %}
    {% endif %}
  }

  {{ cppClassName }}::~{{ cppClassName }}() {
    {% each fields|fieldsInfo as field %}
      {% if not field.ignore %}
        {% if not field.isEnum %}
          {% if field.isLibgitType %}
            this->{{ field.name }}.Reset();
          {% endif %}
        {% endif %}
      {% endif %}
    {% endeach %}
  }

  void {{ cppClassName }}::ConstructFields() {
    Napi::Env constructEnv = nodegitContext->Env();
    {% each fields|fieldsInfo as field %}
      {% if not field.ignore %}
        {% if not field.isEnum %}
          {% if field.isLibgitType %}
            Napi::Object {{ field.name }}Temp = {{ field.cppClassName }}::New(
              {%if not field.cType|isPointer %}&{%endif%}this->raw->{{ field.name }},
              false
            ).As<Napi::Object>();
            this->{{ field.name }} = Napi::Persistent({{ field.name }}Temp);
          {% endif %}
        {% endif %}
      {% endif %}
    {% endeach %}
  }

  {% each fields as field %}
    {% if not field.ignore %}
    {% if not field | isPayload %}
  Napi::Value {{ cppClassName }}_Get{{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info);
  void {{ cppClassName }}_Set{{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info);
    {% endif %}
    {% endif %}
  {% endeach %}

  void {{ cppClassName }}::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
    Napi::Env env = target.Env();
    Napi::HandleScope scope(env);

    Napi::Function tpl = Napi::Function::New(env, JSNewFunction, "{{ jsClassName }}");
    Napi::Object proto = tpl.Get("prototype").As<Napi::Object>();

    {% each fields as field %}
      {% if not field.ignore %}
      {% if not field | isPayload %}
        proto.DefineProperty(Napi::PropertyDescriptor::Accessor(
          env,
          proto,
          "{{ field.jsFunctionName }}",
          {{ cppClassName }}_Get{{ field.cppFunctionName }}_thunk,
          {{ cppClassName }}_Set{{ field.cppFunctionName }}_thunk,
          napi_enumerable));
      {% endif %}
      {% endif %}
    {% endeach %}

    InitializeTemplate(tpl);

    nodegitContext->SaveToPersistent("{{ cppClassName }}::Template", tpl);
    target.Set("{{ jsClassName }}", tpl);
  }

  {% each fields as field %}
    {% if not field.ignore %}
    {% if not field | isPayload %}
  Napi::Value {{ cppClassName }}_Get{{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info) {
    {{ cppClassName }}* self = static_cast<{{ cppClassName }}*>(nodegit::TrackerWrap::Unwrap(info.This().As<Napi::Object>()));
    return self->Get{{ field.cppFunctionName }}(info);
  }
  void {{ cppClassName }}_Set{{ field.cppFunctionName }}_thunk(const Napi::CallbackInfo& info) {
    {{ cppClassName }}* self = static_cast<{{ cppClassName }}*>(nodegit::TrackerWrap::Unwrap(info.This().As<Napi::Object>()));
    self->Set{{ field.cppFunctionName }}(info, info[0]);
  }
    {% endif %}
    {% endif %}
  {% endeach %}

  {% partial fieldAccessors . %}

  // force base class template instantiation, to make sure we get all the
  // methods, statics, etc.
  template class NodeGitWrapper<{{ cppClassName }}Traits>;

{% endif %}

Configurable{{ cppClassName }}::Configurable{{ cppClassName }}(nodegit::Context *nodegitContext)
  : nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>(nodegitContext)
{
  {% if ignoreInit == true %}
    this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}));
  {% else %}
    {{ cType }}{% if isExtendedStruct %}_extended{% endif %} wrappedValue = {{ cType|upper }}_INIT;
    this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}{% if isExtendedStruct %}_extended{% endif %}));
    memcpy(this->raw, &wrappedValue, sizeof({{ cType }}{% if isExtendedStruct %}_extended{% endif %}));
  {% endif %}
}

Configurable{{ cppClassName }}::~Configurable{{ cppClassName }}() {
  {% each fields|fieldsInfo as field %}
    {% if not field.ignore %}
      {% if field.cppClassName == 'GitStrarray' %}
        if (this->raw->{{ field.name }}.count) {
          for (size_t i = 0; i < this->raw->{{ field.name }}.count; ++i) {
            free(this->raw->{{ field.name }}.strings[i]);
          }
          free(this->raw->{{ field.name }}.strings);
        }
      {% elsif field.cppClassName == 'String' %}
        free((void*)this->raw->{{ field.name }});
      {% endif %}
    {% endif %}
  {% endeach %}
}

nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>::v8ConversionResult Configurable{{ cppClassName }}::fromJavascript(nodegit::Context *nodegitContext, Napi::Value input) {
  if (!input.IsObject()) {
    return {
      "Must pass object for Configurable{{ cppClassName }}"
    };
  }

  Napi::Env env = nodegitContext->Env();
  Napi::Object inputObj = input.As<Napi::Object>();
  std::shared_ptr<Configurable{{ cppClassName }}> output(new Configurable{{ cppClassName }}(nodegitContext));

  // unpack the data into the correct fields
  {% each fields as field %}
    {% if not field.ignore %}
      {% if field.isClassType %}
        {% if field.cppClassName == 'GitOid' %}
          {
            Napi::Value maybeOid = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeOid.IsUndefined() && !maybeOid.IsNull()) {
              if (maybeOid.IsString()) {
                Napi::String oidStringStr = maybeOid.As<Napi::String>();
                std::string oidString = oidStringStr.Utf8Value();
                if (git_oid_fromstr(&output->raw->{{ field.name }}, oidString.c_str()) != GIT_OK) {
                  return {
                    git_error_last()->message
                  };
                }
              } else if (maybeOid.IsObject()) {
                if (git_oid_cpy(&output->raw->{{ field.name }}, NodeGitWrapper<{{ field.cppClassName }}Traits>::Unwrap<{{ field.cppClassName }}>(maybeOid.As<Napi::Object>())->GetValue()) != GIT_OK) {
                  return {
                    git_error_last()->message
                  };
                }
              } else {
                return {
                  "Must pass String or NodeGit.Oid to {{ field.jsFunctionName }}"
                };
              }
            }
          }
        {% elsif field.cppClassName == 'GitStrarray' %}
          output->raw->{{ field.name }}.count = 0;
          output->raw->{{ field.name }}.strings = nullptr;

          {
            Napi::Value maybeStrarray = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeStrarray.IsUndefined() && !maybeStrarray.IsNull()) {
              if (maybeStrarray.IsArray()) {
                Napi::Array strarrayValue = maybeStrarray.As<Napi::Array>();
                // validate the StrArray is indeed a list of strings
                for (uint32_t i = 0; i < strarrayValue.Length(); ++i) {
                  // TODO confirm that sparse array at least boils down to undefined
                  Napi::Value arrayValue = strarrayValue.Get(i);
                  if (!arrayValue.IsString()) {
                    return {
                      "Must pass String or Array of strings to {{ field.jsFunctionName }}"
                    };
                  }
                }

                StrArrayConverter::ConvertInto(&output->raw->{{ field.name }}, strarrayValue);
              } else if (maybeStrarray.IsString()) {
                Napi::String strarrayValue = maybeStrarray.As<Napi::String>();
                StrArrayConverter::ConvertInto(&output->raw->{{ field.name }}, strarrayValue);
              } else {
                return {
                  "Must pass String or Array of strings to {{ field.jsFunctionName }}"
                };
              }
            }
          }
        {% else %}
          {
            Napi::Value maybeObject = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeObject.IsUndefined() && !maybeObject.IsNull()) {
              if (!maybeObject.IsObject()) {
                return {
                  "Must pass NodeGit.{{ field.jsClassName }} to {{ field.jsFunctionName }}"
                };
              }

              Napi::Object objectValue = maybeObject.As<Napi::Object>();
              output->raw->{{ field.name }} = NodeGitWrapper<{{ field.cppClassName }}Traits>::Unwrap<{{ field.cppClassName }}>(objectValue)->GetValue();
              output->{{ field.jsFunctionName }} = Napi::Persistent(objectValue);
            }
          }
        {% endif %}
      {% elsif field.isCallbackFunction %}
        {
          Napi::Value maybeCallback = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeCallback.IsUndefined() && !maybeCallback.IsNull()) {
            if (!maybeCallback.IsFunction() && !maybeCallback.IsObject()) {
              return {
                "Must pass Function or CallbackSpecifier to {{ field.jsFunctionName}}"
              };
            }

            std::unique_ptr<Napi::FunctionReference> callback;
            uint32_t throttle = {% if field.return.throttle %}{{ field.return.throttle }}{% else %}0{% endif %};
            bool waitForResult = true;

            if (maybeCallback.IsFunction()) {
              callback.reset(new Napi::FunctionReference(Napi::Persistent(maybeCallback.As<Napi::Function>())));
            } else {
              Napi::Object callbackSpecifier = maybeCallback.As<Napi::Object>();
              Napi::Value maybeCallbackInner = nodegit::safeGetField(callbackSpecifier, "callback");
              if (!maybeCallbackInner.IsFunction()) {
                return {
                  "Must pass callback to CallbackSpecifier"
                };
              }

              callback.reset(new Napi::FunctionReference(Napi::Persistent(maybeCallbackInner.As<Napi::Function>())));

              Napi::Value maybeThrottle = nodegit::safeGetField(callbackSpecifier, "throttle");
              if (!maybeThrottle.IsUndefined() && !maybeThrottle.IsNull()) {
                if (!maybeThrottle.IsNumber()) {
                  return {
                    "Must pass zero or positive number as throttle to CallbackSpecifier"
                  };
                }

                throttle = maybeThrottle.As<Napi::Number>().Uint32Value();
              }

              Napi::Value maybeWaitForResult = nodegit::safeGetField(callbackSpecifier, "waitForResult");
              if (!maybeWaitForResult.IsUndefined() && !maybeWaitForResult.IsNull()) {
                if (!maybeWaitForResult.IsBoolean()) {
                  return {
                    "Must pass a boolean as waitForResult to callbackSpecifier"
                  };
                }

                waitForResult = maybeWaitForResult.ToBoolean().Value();
              }
            }

            output->{{ field.jsFunctionName }}.SetCallback(std::move(callback), throttle, waitForResult);
            output->raw->{{ field.name }} = ({{ field.cType }}){{ field.jsFunctionName }}_cppCallback;
          }
        }
      {% elsif field.isStructType %}
        {
          Napi::Value maybeNestedObject = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNestedObject.IsUndefined() && !maybeNestedObject.IsNull()) {
            auto conversionResult = Configurable{{ field.cppClassName }}::fromJavascript(nodegitContext, maybeNestedObject);
            if (!conversionResult.result) {
              std::string error = "Failed to set {{ field.jsFunctionName }}: ";
              error += conversionResult.error;
              return {
                error
              };
            }

            auto child = conversionResult.result;
            output->childCleanupVector.push_back(child);
            output->raw->{{ field.name }} = *child->GetValue();
          }
        }
      {% elsif field.payloadFor %}
        output->raw->{{ field.name }} = (void *)output.get();
      {% elsif field.cppClassName == 'String' %}
        output->raw->{{ field.name }} = nullptr;
        {
          Napi::Value maybeString = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeString.IsUndefined() && !maybeString.IsNull()) {
            if (!maybeString.IsString()) {
              return {
                "Must pass string to {{ field.jsFunctionName }}"
              };
            }

            Napi::String sStr = maybeString.As<Napi::String>();
            std::string utf8String = sStr.Utf8Value();
            output->raw->{{ field.name }} = strdup(utf8String.c_str());
          }
        }
      {% elsif field.isCppClassIntType %}
        {
          Napi::Value maybeNumber = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNumber.IsUndefined() && !maybeNumber.IsNull()) {
            if (!maybeNumber.IsNumber()) {
              return {
                "Must pass {{ field.cppClassName }} to {{ field.jsFunctionName }}"
              };
            }

            output->raw->{{ field.name }} = maybeNumber.As<Napi::Number>().Int32Value();
          }
        }
      {% else %}
        {
          Napi::Value maybeNumber = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNumber.IsUndefined() && !maybeNumber.IsNull()) {
            if (!maybeNumber.IsNumber()) {
              return {
                "Must pass Int32 to {{ field.jsFunctionName }}"
              };
            }

            output->raw->{{ field.name }} = static_cast<{{ field.cType }}>(maybeNumber.As<Napi::Number>().Int32Value());
          }
        }
      {% endif %}
    {% endif %}
  {% endeach %}

  {% if isExtendedStruct %}
    (({{ cType }}_extended *)output->raw)->payload = (void *)output.get();
  {% endif %}

  return {
    output
  };
}

{% partial configurableCallbacks %}

// force base class template instantiation, to make sure we get all the
// methods, statics, etc.
template class nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>;
