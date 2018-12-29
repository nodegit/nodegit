{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {

      {{ cppClassName }} *wrapper = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

      {% if field.isEnum %}
        info.GetReturnValue().Set(Nan::New((int)wrapper->GetValue()->{{ field.name }}));

      {% elsif field.isLibgitType | or field.payloadFor %}
        info.GetReturnValue().Set(Nan::New(wrapper->{{ field.name }}));

      {% elsif field.isCallbackFunction %}
        if (wrapper->{{field.name}}.HasCallback()) {
          info.GetReturnValue().Set(wrapper->{{ field.name }}.GetCallback()->GetFunction());
        } else {
          info.GetReturnValue().SetUndefined();
        }

      {% elsif field.cppClassName == 'String' %}
        if (wrapper->GetValue()->{{ field.name }}) {
          info.GetReturnValue().Set(Nan::New<String>(wrapper->GetValue()->{{ field.name }}).ToLocalChecked());
        }
        else {
          return;
        }

      {% elsif field.cppClassName|isV8Value %}
        info.GetReturnValue().Set(Nan::New<{{ field.cppClassName }}>(wrapper->GetValue()->{{ field.name }}));
      {% endif %}
    }

    NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
      {{ cppClassName }} *wrapper = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

      {% if field.isEnum %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) Nan::To<int32_t>(value).FromJust();
        }

      {% elsif field.isLibgitType %}
        v8::Local<Object> {{ field.name }}(value->ToObject());

        wrapper->{{ field.name }}.Reset({{ field.name }});

        wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}{% if field.cppClassName == 'GitStrarray' %}StrArrayConverter::Convert({{ field.name }}->ToObject()){% else %}Nan::ObjectWrap::Unwrap<{{ field.cppClassName }}>({{ field.name }}->ToObject())->GetValue(){% endif %};

      {% elsif field.isCallbackFunction %}
        Nan::Callback *callback = NULL;
        int throttle = {%if field.return.throttle %}{{ field.return.throttle }}{%else%}0{%endif%};
        bool waitForResult = true;

        if (value->IsFunction()) {
          callback = new Nan::Callback(value.As<Function>());
        } else if (value->IsObject()) {
          v8::Local<Object> object = value.As<Object>();
          v8::Local<String> callbackKey;
          Nan::MaybeLocal<Value> maybeObjectCallback = Nan::Get(object, Nan::New("callback").ToLocalChecked());
          if (!maybeObjectCallback.IsEmpty()) {
            v8::Local<Value> objectCallback = maybeObjectCallback.ToLocalChecked();
            if (objectCallback->IsFunction()) {
              callback = new Nan::Callback(objectCallback.As<Function>());

              Nan::MaybeLocal<Value> maybeObjectThrottle = Nan::Get(object, Nan::New("throttle").ToLocalChecked());
              if(!maybeObjectThrottle.IsEmpty()) {
                v8::Local<Value> objectThrottle = maybeObjectThrottle.ToLocalChecked();
                if (objectThrottle->IsNumber()) {
                  throttle = (int)objectThrottle.As<Number>()->Value();
                }
              }

              Nan::MaybeLocal<Value> maybeObjectWaitForResult = Nan::Get(object, Nan::New("waitForResult").ToLocalChecked());
              if(!maybeObjectWaitForResult.IsEmpty()) {
                Local<Value> objectWaitForResult = maybeObjectWaitForResult.ToLocalChecked();
                waitForResult = (bool)objectWaitForResult->BooleanValue();
              }
            }
          }
        }
        if (callback) {
          if (!wrapper->raw->{{ field.name }}) {
            wrapper->raw->{{ field.name }} = ({{ field.cType }}){{ field.name }}_cppCallback;
          }

          wrapper->{{ field.name }}.SetCallback(callback, throttle, waitForResult);
        }

      {% elsif field.payloadFor %}
        wrapper->{{ field.name }}.Reset(value);

      {% elsif field.cppClassName == 'String' %}
        if (wrapper->GetValue()->{{ field.name }}) {
        }

        String::Utf8Value str(value);
        wrapper->GetValue()->{{ field.name }} = strdup(*str);

      {% elsif field.isCppClassIntType %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = value->{{field.cppClassName}}Value();
        }

      {% else %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) Nan::To<int32_t>(value).FromJust();
        }
      {% endif %}
    }

    {% if field.isCallbackFunction %}
      {{ cppClassName }}* {{ cppClassName }}::{{ field.name }}_getInstanceFromBaton({{ field.name|titleCase }}Baton* baton) {
        {% if isExtendedStruct %}
          return static_cast<{{ cppClassName }}*>((({{cType}}_extended *)baton->self)->payload);
        {% else %}
          return static_cast<{{ cppClassName }}*>(baton->
          {% each field.args|argsInfo as arg %}
            {% if arg.payload == true %}
              {{arg.name}}
            {% elsif arg.lastArg %}
              {{arg.name}}
            {% endif %}
          {% endeach %});
        {% endif %}
      }

      {{ field.return.type }} {{ cppClassName }}::{{ field.name }}_cppCallback (
        {% each field.args|argsInfo as arg %}
          {{ arg.cType }} {{ arg.name}}{% if not arg.lastArg %},{% endif %}
        {% endeach %}
      ) {
        {{ field.name|titleCase }}Baton *baton =
          new {{ field.name|titleCase }}Baton({{ field.return.noResults }});

        {% each field.args|argsInfo as arg %}
          baton->{{ arg.name }} = {{ arg.name }};
        {% endeach %}

        {{ cppClassName }}* instance = {{ field.name }}_getInstanceFromBaton(baton);

        {% if field.return.type == "void" %}
          if (instance->{{ field.name }}.WillBeThrottled()) {
            delete baton;
          } else if (instance->{{ field.name }}.ShouldWaitForResult()) {
            baton->ExecuteAsync({{ field.name }}_async);
            delete baton;
          } else {
            baton->ExecuteAsync({{ field.name }}_async, deleteBaton);
          }
          return;
        {% else %}
          {{ field.return.type }} result;

          if (instance->{{ field.name }}.WillBeThrottled()) {
            result = baton->defaultResult;
            delete baton;
          } else if (instance->{{ field.name }}.ShouldWaitForResult()) {
            result = baton->ExecuteAsync({{ field.name }}_async);
            delete baton;
          } else {
            result = baton->defaultResult;
            baton->ExecuteAsync({{ field.name }}_async, deleteBaton);
          }
          return result;
        {% endif %}
      }


      void {{ cppClassName }}::{{ field.name }}_async(void *untypedBaton) {
        Nan::HandleScope scope;

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(untypedBaton);
        {{ cppClassName }}* instance = {{ field.name }}_getInstanceFromBaton(baton);

        if (instance->{{ field.name }}.GetCallback()->IsEmpty()) {
          {% if field.return.type == "int" %}
            baton->result = baton->defaultResult; // no results acquired
          {% endif %}
          baton->Done();
          return;
        }

        {% each field.args|argsInfo as arg %}
          {% if arg.name == "payload" %}
            {%-- Do nothing --%}
          {% elsif arg.isJsArg %}
            {% if arg.cType == "const char *" %}
              if (baton->{{ arg.name }} == NULL) {
                  baton->{{ arg.name }} = "";
              }
            {% elsif arg.cppClassName == "String" %}
              v8::Local<v8::Value> src;
              if (baton->{{ arg.name }} == NULL) {
                  src = Nan::Null();
              }
              else {
                src = Nan::New<String>(*baton->{{ arg.name }}).ToLocalChecked();
              }
            {% endif %}
          {% endif %}
        {% endeach %}

        {% if field.isSelfReferential %}
          {% if field.args|jsArgsCount|subtract 2| setUnsigned == 0 %}
            v8::Local<Value> *argv = NULL;
          {% else %}
            v8::Local<Value> argv[{{ field.args|jsArgsCount|subtract 2| setUnsigned }}] = {
          {% endif %}
        {% else %}
          v8::Local<Value> argv[{{ field.args|jsArgsCount }}] = {
        {% endif %}
        {% each field.args|argsInfo as arg %}
          {% if field.isSelfReferential %}
            {% if not arg.firstArg %}
              {% if field.args|jsArgsCount|subtract 1|or 0 %}
                {% if arg.cppClassName == "String" %}
                  {%-- src is always the last arg --%}
                  src
                {% elsif arg.isJsArg %}
                  {% if arg.isEnum %}
                    Nan::New((int)baton->{{ arg.name }}),
                  {% elsif arg.isLibgitType %}
                    {{ arg.cppClassName }}::New(baton->{{ arg.name }}, false),
                  {% elsif arg.cType == "size_t" %}
                    Nan::New((unsigned int)baton->{{ arg.name }}),
                  {% elsif arg.name == "payload" %}
                    {%-- skip, filters should not have a payload --%}
                  {% else %}
                    Nan::New(baton->{{ arg.name }}),
                  {% endif %}
                {% endif %}
              {% endif %}
            {% endif %}
          {% else %}
            {% if arg.name == "payload" %}
              {%-- payload is always the last arg --%}
              Nan::New(instance->{{ fields|payloadFor field.name }})
            {% elsif arg.isJsArg %}
              {% if arg.isEnum %}
                Nan::New((int)baton->{{ arg.name }}),
              {% elsif arg.isLibgitType %}
                {{ arg.cppClassName }}::New(baton->{{ arg.name }}, false),
              {% elsif arg.cType == "size_t" %}
                // HACK: NAN should really have an overload for Nan::New to support size_t
                Nan::New((unsigned int)baton->{{ arg.name }}),
              {% elsif arg.cppClassName == "String" %}
                Nan::New(baton->{{ arg.name }}).ToLocalChecked(),
              {% else %}
                Nan::New(baton->{{ arg.name }}),
              {% endif %}
            {% endif %}
          {% endif %}
        {% endeach %}
        {% if not field.isSelfReferential %}
          };
        {% elsif field.args|jsArgsCount|subtract 2| setUnsigned > 0  %}
          };
        {% endif %}

        Nan::TryCatch tryCatch;

        // TODO This should take an async_resource, but we will need to figure out how to pipe the correct context into this
        {% if field.isSelfReferential %}
          Nan::MaybeLocal<v8::Value> maybeResult = Nan::Call(*(instance->{{ field.name }}.GetCallback()), {{ field.args|jsArgsCount|subtract 2| setUnsigned }}, argv);
        {% else  %}
          Nan::MaybeLocal<v8::Value> maybeResult = Nan::Call(*(instance->{{ field.name }}.GetCallback()), {{ field.args|jsArgsCount }}, argv);
        {% endif %}

        v8::Local<v8::Value> result;
        if (!maybeResult.IsEmpty()) {
          result = maybeResult.ToLocalChecked();
        }

        if(PromiseCompletion::ForwardIfPromise(result, baton, {{ cppClassName }}::{{ field.name }}_promiseCompleted)) {
          return;
        }

        {% if field.return.type == "void" %}
          baton->Done();
        {% else %}
          {% each field|returnsInfo false true as _return %}
            if (result.IsEmpty() || result->IsNativeError()) {
              baton->result = {{ field.return.error }};
            }
            else if (!result->IsNull() && !result->IsUndefined()) {
              {% if _return.isOutParam %}
              {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
              wrapper->selfFreeing = false;

              *baton->{{ _return.name }} = wrapper->GetValue();
              baton->result = {{ field.return.success }};
              {% else %}
              if (result->IsNumber()) {
                baton->result = Nan::To<int>(result).FromJust();
              }
              else {
                baton->result = baton->defaultResult;
              }
              {% endif %}
            }
            else {
              baton->result = baton->defaultResult;
            }
          {% endeach %}
          baton->Done();
        {% endif %}
      }

      void {{ cppClassName }}::{{ field.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result) {
        Nan::HandleScope scope;

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(_baton);
        {% if field.return.type == "void" %}
          baton->Done();
        {% else %}
          if (isFulfilled) {
            {% each field|returnsInfo false true as _return %}
              if (result.IsEmpty() || result->IsNativeError()) {
                baton->result = {{ field.return.error }};
              }
              else if (!result->IsNull() && !result->IsUndefined()) {
                {% if _return.isOutParam %}
                {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
                wrapper->selfFreeing = false;

                *baton->{{ _return.name }} = wrapper->GetValue();
                baton->result = {{ field.return.success }};
                {% else %}
                if (result->IsNumber()) {
                  baton->result = Nan::To<int>(result).FromJust();
                }
                else{
                  baton->result = baton->defaultResult;
                }
                {% endif %}
              }
              else {
                baton->result = baton->defaultResult;
              }
            {% endeach %}
          }
          else {
            // promise was rejected
            {% if isExtendedStruct %}
              {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>((({{cType}}_extended *)baton->self)->payload);
            {% else %}
              {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->{% each field.args|argsInfo as arg %}
              {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
              {% endeach %});
            {% endif %}
            v8::Local<v8::Object> parent = instance->handle();
            SetPrivate(parent, Nan::New("NodeGitPromiseError").ToLocalChecked(), result);

            baton->result = {{ field.return.error }};
          }
          baton->Done();
        {% endif %}
      }
    {% endif %}
  {% endif %}
{% endeach %}
