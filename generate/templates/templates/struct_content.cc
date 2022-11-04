// generated from struct_content.cc
#include <nan.h>
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
  {{ cppClassName }}::{{ cppClassName }}() : NodeGitWrapper<{{ cppClassName }}Traits>(NULL, true, v8::Local<v8::Object>())
  {
    {% if ignoreInit == true %}
    this->raw = new {{ cType }};
    {% else %}
      {% if isExtendedStruct %}
        {{ cType }}_extended wrappedValue = {{ cType|upper }}_INIT;
        this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}_extended));
        memcpy(this->raw, &wrappedValue, sizeof({{ cType }}_extended));
      {% else %}
        {{ cType }} wrappedValue = {{ cType|upper }}_INIT;
        this->raw = ({{ cType }}*) malloc(sizeof({{ cType }}));
        memcpy(this->raw, &wrappedValue, sizeof({{ cType }}));
      {% endif %}
    {% endif %}

    this->ConstructFields();
  }

  {{ cppClassName }}::{{ cppClassName }}({{ cType }}* raw, bool selfFreeing, v8::Local<v8::Object> owner)
   : NodeGitWrapper<{{ cppClassName }}Traits>(raw, selfFreeing, owner)
  {
    this->ConstructFields();
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
    {% each fields|fieldsInfo as field %}
      {% if not field.ignore %}
        {% if not field.isEnum %}
          {% if field.isLibgitType %}
            v8::Local<Object> {{ field.name }}Temp = Nan::To<v8::Object>({{ field.cppClassName }}::New(
              {%if not field.cType|isPointer %}&{%endif%}this->raw->{{ field.name }},
              false
            )).ToLocalChecked();
            this->{{ field.name }}.Reset({{ field.name }}Temp);
          {% endif %}
        {% endif %}
      {% endif %}
    {% endeach %}
  }

  void {{ cppClassName }}::InitializeComponent(Local<Object> target, nodegit::Context *nodegitContext) {
    Nan::HandleScope scope;

    Local<External> nodegitExternal = Nan::New<External>(nodegitContext);
    Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction, nodegitExternal);

    tpl->InstanceTemplate()->SetInternalFieldCount(2);
    tpl->SetClassName(Nan::New("{{ jsClassName }}").ToLocalChecked());

    {% each fields as field %}
      {% if not field.ignore %}
      {% if not field | isPayload %}
        Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("{{ field.jsFunctionName }}").ToLocalChecked(), Get{{ field.cppFunctionName}}, Set{{ field.cppFunctionName}}, nodegitExternal);
      {% endif %}
      {% endif %}
    {% endeach %}

    InitializeTemplate(tpl);

    v8::Local<Function> constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
    nodegitContext->SaveToPersistent("{{ cppClassName }}::Template", constructor_template);
  }

  {% partial fieldAccessors . %}

  // force base class template instantiation, to make sure we get all the
  // methods, statics, etc.
  template class NodeGitWrapper<{{ cppClassName }}Traits>;

{% endif %}

Configurable{{ cppClassName }}::Configurable{{ cppClassName }}(nodegit::Context *nodegitContext)
  : nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>(nodegitContext)
{
  {% if ignoreInit == true %}
    this->raw = new {{ cType }};
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
            delete this->raw->{{ field.name }}.strings[i];
          }
          delete[] this->raw->{{ field.name }}.strings;
        }
      {% elsif field.cppClassName == 'String' %}
        delete this->raw->{{ field.name }};
      {% endif %}
    {% endif %}
  {% endeach %}
}

nodegit::ConfigurableClassWrapper<{{ cppClassName }}Traits>::v8ConversionResult Configurable{{ cppClassName }}::fromJavascript(nodegit::Context *nodegitContext, v8::Local<v8::Value> input) {
  if (!input->IsObject()) {
    return {
      "Must pass object for Configurable{{ cppClassName }}"
    };
  }

  Nan::HandleScope scope;
  v8::Local<v8::Object> inputObj = input.As<v8::Object>();
  std::shared_ptr<Configurable{{ cppClassName }}> output(new Configurable{{ cppClassName }}(nodegitContext));

  // unpack the data into the correct fields
  {% each fields as field %}
    {% if not field.ignore %}
      {% if field.isClassType %}
        {% if field.cppClassName == 'GitOid' %}
          {
            v8::Local<v8::Value> maybeOid = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeOid.IsEmpty() && !maybeOid->IsUndefined() && !maybeOid->IsNull()) {
              if (maybeOid->IsString()) {
                Nan::Utf8String oidString(maybeOid.As<v8::String>());
                if (git_oid_fromstr(&output->raw->{{ field.name }}, *oidString) != GIT_OK) {
                  return {
                    git_error_last()->message
                  };
                }
              } else if (maybeOid->IsObject()) {
                if (git_oid_cpy(&output->raw->{{ field.name }}, Nan::ObjectWrap::Unwrap<{{ field.cppClassName }}>(maybeOid.As<v8::Object>())->GetValue()) != GIT_OK) {
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
            v8::Local<v8::Value> maybeStrarray = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeStrarray.IsEmpty() && !maybeStrarray->IsUndefined() && !maybeStrarray->IsNull()) {
              if (maybeStrarray->IsArray()) {
                v8::Local<v8::Array> strarrayValue = maybeStrarray.As<v8::Array>();
                // validate the StrArray is indeed a list of strings
                for (uint32_t i = 0; i < strarrayValue->Length(); ++i) {
                  // TODO confirm that sparse array at least boils down to undefined
                  v8::Local<v8::Value> arrayValue = Nan::Get(strarrayValue, i).ToLocalChecked();
                  if (!arrayValue->IsString()) {
                    return {
                      "Must pass String or Array of strings to {{ field.jsFunctionName }}"
                    };
                  }
                }

                StrArrayConverter::ConvertInto(&output->raw->{{ field.name }}, strarrayValue);
              } else if (maybeStrarray->IsString()) {
                v8::Local<v8::String> strarrayValue = maybeStrarray.As<v8::String>();
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
            v8::Local<v8::Value> maybeObject = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
            if (!maybeObject.IsEmpty() && !maybeObject->IsUndefined() && !maybeObject->IsNull()) {
              if (!maybeObject->IsObject()) {
                return {
                  "Must pass NodeGit.{{ field.jsClassName }} to {{ field.jsFunctionName }}"
                };
              }

              v8::Local<v8::Object> objectValue = maybeObject.As<v8::Object>();
              output->raw->{{ field.name }} = Nan::ObjectWrap::Unwrap<{{ field.cppClassName }}>(objectValue)->GetValue();
              output->{{ field.jsFunctionName }}.Reset(objectValue);
            }
          }
        {% endif %}
      {% elsif field.isCallbackFunction %}
        {
          v8::Local<v8::Value> maybeCallback = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeCallback.IsEmpty() && !maybeCallback->IsUndefined() && !maybeCallback->IsNull()) {
            if (!maybeCallback->IsFunction() && !maybeCallback->IsObject()) {
              return {
                "Must pass Function or CallbackSpecifier to {{ field.jsFunctionName}}"
              };
            }

            std::unique_ptr<Nan::Callback> callback;
            uint32_t throttle = {% if field.return.throttle %}{{ field.return.throttle }}{% else %}0{% endif %};
            bool waitForResult = true;

            if (maybeCallback->IsFunction()) {
              callback.reset(new Nan::Callback(maybeCallback.As<v8::Function>()));
            } else {
              v8::Local<v8::Object> callbackSpecifier = maybeCallback.As<v8::Object>();
              v8::Local<v8::Value> maybeCallback = nodegit::safeGetField(callbackSpecifier, "callback");
              if (maybeCallback.IsEmpty() || !maybeCallback->IsFunction()) {
                return {
                  "Must pass callback to CallbackSpecifier"
                };
              }

              callback.reset(new Nan::Callback(maybeCallback.As<v8::Function>()));

              v8::Local<v8::Value> maybeThrottle = nodegit::safeGetField(callbackSpecifier, "throttle");
              if (!maybeThrottle.IsEmpty() && !maybeThrottle->IsUndefined() && !maybeThrottle->IsNull()) {
                if (!maybeThrottle->IsNumber()) {
                  return {
                    "Must pass zero or positive number as throttle to CallbackSpecifier"
                  };
                }

                throttle = maybeThrottle->Uint32Value(Nan::GetCurrentContext()).FromJust();
              }

              v8::Local<v8::Value> maybeWaitForResult = nodegit::safeGetField(callbackSpecifier, "waitForResult");
              if (!maybeWaitForResult.IsEmpty() && !maybeWaitForResult->IsUndefined() && !maybeWaitForResult->IsNull()) {
                if (!maybeWaitForResult->IsBoolean()) {
                  return {
                    "Must pass a boolean as waitForResult to callbackSpecifier"
                  };
                }

                waitForResult = Nan::To<bool>(maybeWaitForResult).FromJust();
              }
            }

            output->{{ field.jsFunctionName }}.SetCallback(std::move(callback), throttle, waitForResult);
            output->raw->{{ field.name }} = ({{ field.cType }}){{ field.jsFunctionName }}_cppCallback;
          }
        }
      {% elsif field.isStructType %}
        {
          v8::Local<v8::Value> maybeNestedObject = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNestedObject.IsEmpty() && !maybeNestedObject->IsUndefined() && !maybeNestedObject->IsNull()) {
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
          v8::Local<v8::Value> maybeString = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeString.IsEmpty() && !maybeString->IsUndefined() && !maybeString->IsNull()) {
            if (!maybeString->IsString()) {
              return {
                "Must pass string to {{ field.jsFunctionName }}"
              };
            }

            Nan::Utf8String utf8String(maybeString.As<v8::String>());
            output->raw->{{ field.name }} = strdup(*utf8String);
          }
        }
      {% elsif field.isCppClassIntType %}
        {
          v8::Local<v8::Value> maybeNumber = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNumber.IsEmpty() && !maybeNumber->IsUndefined() && !maybeNumber->IsNull()) {
            if (!maybeNumber->IsNumber()) {
              return {
                "Must pass {{ field.cppClassName }} to {{ field.jsFunctionName }}"
              };
            }

            output->raw->{{ field.name }} = maybeNumber->{{ field.cppClassName }}Value();
          }
        }
      {% else %}
        {
          v8::Local<v8::Value> maybeNumber = nodegit::safeGetField(inputObj, "{{ field.jsFunctionName }}");
          if (!maybeNumber.IsEmpty() && !maybeNumber->IsUndefined() && !maybeNumber->IsNull()) {
            if (!maybeNumber->IsNumber()) {
              return {
                "Must pass Int32 to {{ field.jsFunctionName }}"
              };
            }

            output->raw->{{ field.name }} = static_cast<{{ field.cType }}>(maybeNumber->Int32Value(Nan::GetCurrentContext()).FromJust());
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
