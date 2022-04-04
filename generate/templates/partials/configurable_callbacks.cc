{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    {% if field.isCallbackFunction %}
      Configurable{{ cppClassName }}* Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_getInstanceFromBaton({{ field.name|titleCase }}Baton* baton) {
        {% if isExtendedStruct %}
          return static_cast<Configurable{{ cppClassName }}*>((({{cType}}_extended *)baton->self)->payload);
        {% else %}
          return static_cast<Configurable{{ cppClassName }}*>(baton->
          {% each field.args|argsInfo as arg %}
            {% if arg.payload == true %}
              {{arg.name}}
            {% elsif arg.lastArg %}
              {{arg.name}}
            {% endif %}
          {% endeach %});
        {% endif %}
      }

      {{ field.return.type }} Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_cppCallback (
        {% each field.args|argsInfo as arg %}
          {{ arg.cType }} {{ arg.name}}{% if not arg.lastArg %},{% endif %}
        {% endeach %}
      ) {
        {{ field.name|titleCase }}Baton *baton =
          new {{ field.name|titleCase }}Baton({{ field.return.noResults }});

        {% each field.args|argsInfo as arg %}
          baton->{{ arg.name }} = {{ arg.name }};
        {% endeach %}

        Configurable{{ cppClassName }}* instance = {{ field.jsFunctionName }}_getInstanceFromBaton(baton);

        {% if field.return.type == "void" %}
          if (instance->nodegitContext != nodegit::ThreadPool::GetCurrentContext()) {
            delete baton;
          } else if (instance->{{ field.jsFunctionName }}.WillBeThrottled()) {
            delete baton;
          } else if (instance->{{ field.jsFunctionName }}.ShouldWaitForResult()) {
            baton->ExecuteAsync({{ field.jsFunctionName }}_async, {{ field.jsFunctionName }}_cancelAsync);
            delete baton;
          } else {
            baton->ExecuteAsync({{ field.jsFunctionName }}_async, {{ field.jsFunctionName }}_cancelAsync, nodegit::deleteBaton);
          }
          return;
        {% else %}
          {{ field.return.type }} result;

          if (instance->nodegitContext != nodegit::ThreadPool::GetCurrentContext()) {
            result = baton->defaultResult;
            delete baton;
          } else if (instance->{{ field.jsFunctionName }}.WillBeThrottled()) {
            result = baton->defaultResult;
            delete baton;
          } else if (instance->{{ field.jsFunctionName }}.ShouldWaitForResult()) {
            result = baton->ExecuteAsync({{ field.jsFunctionName }}_async, {{ field.jsFunctionName }}_cancelAsync);
            delete baton;
          } else {
            result = baton->defaultResult;
            baton->ExecuteAsync({{ field.jsFunctionName }}_async, {{ field.jsFunctionName }}_cancelAsync, nodegit::deleteBaton);
          }
          return result;
        {% endif %}
      }

      void Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_cancelAsync(void *untypedBaton) {
        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(untypedBaton);
        {% if field.return.type != "void" %}
          baton->result = {{ field.return.cancel }};
        {% endif %}
        baton->Done();
      }

      void Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_async(void *untypedBaton) {
        Nan::HandleScope scope;

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(untypedBaton);
        Configurable{{ cppClassName }}* instance = {{ field.jsFunctionName }}_getInstanceFromBaton(baton);

        if (instance->{{ field.jsFunctionName }}.GetCallback()->IsEmpty()) {
          {% if field.return.type == "int" %}
            baton->result = baton->defaultResult; // no results acquired
          {% endif %}
          baton->Done();
          return;
        }

        {% each field.args|callbackArgsInfo as arg %}
        {% if arg.cppClassName == "Array" %}
          v8::Local<v8::Array> _{{arg.name}}_array = Nan::New<v8::Array>(baton->{{ arg.arrayLengthArgumentName }});
          for(uint32_t i = 0; i < _{{arg.name}}_array->Length(); i++) {
            Nan::Set(_{{arg.name}}_array, i, {{arg.arrayElementCppClassName}}::New(baton->{{arg.name}}[i], false));
          }
        {% endif %}
        {% endeach %}

        {% if field.args|callbackArgsCount == 0 %}
          v8::Local<v8::Value> *argv = NULL;
        {% else %}
          v8::Local<v8::Value> argv[{{ field.args|callbackArgsCount }}] = {
            {% each field.args|callbackArgsInfo as arg %}
            {% if not arg.firstArg %},{% endif %}
            {% if arg.isEnum %}
              Nan::New((int)baton->{{ arg.name }})
            {% elsif arg.cppClassName == "Array" %}
              _{{arg.name}}_array
            {% elsif arg.isLibgitType %}
              {{ arg.cppClassName }}::New(baton->{{ arg.name }}, false)
            {% elsif arg.cType == "size_t" %}
              // HACK: NAN should really have an overload for Nan::New to support size_t
              Nan::New((unsigned int)baton->{{ arg.name }})
            {% elsif arg.cppClassName == "String" %}
              baton->{{ arg.name }} == NULL
                ? Nan::EmptyString()
                : Nan::New({%if arg.cType | isDoublePointer %}*{% endif %}baton->{{ arg.name }}).ToLocalChecked()
            {% else %}
              Nan::New(baton->{{ arg.name }})
            {% endif %}
            {% endeach %}
          };
        {% endif %}

        Nan::TryCatch tryCatch;

        Nan::MaybeLocal<v8::Value> maybeResult = (*(instance->{{ field.jsFunctionName }}.GetCallback()))(
          baton->GetAsyncResource(),
          {{ field.args|callbackArgsCount }},
          argv
        );
        v8::Local<v8::Value> result;
        if (!maybeResult.IsEmpty()) {
          result = maybeResult.ToLocalChecked();
        }

        if (PromiseCompletion::ForwardIfPromise(result, baton, Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_promiseCompleted)) {
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
                {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(Nan::To<v8::Object>(result).ToLocalChecked());
                wrapper->selfFreeing = false;

                {% if _return.cppClassName == "GitOid" %}
                  git_oid_cpy(baton->{{ _return.name }}, wrapper->GetValue());
                {% else %}
                  *baton->{{ _return.name }} = wrapper->GetValue();
                {% endif %}
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

      void Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, v8::Local<v8::Value> result) {
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
                  {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(Nan::To<v8::Object>(result).ToLocalChecked());
                  wrapper->selfFreeing = false;

                  {% if _return.cppClassName == "GitOid" %}
                    git_oid_cpy(baton->{{ _return.name }}, wrapper->GetValue());
                  {% else %}
                    *baton->{{ _return.name }} = wrapper->GetValue();
                  {% endif %}
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
          }
          else {
            // promise was rejected
            {% if isExtendedStruct %}
              Configurable{{ cppClassName }}* instance = static_cast<Configurable{{ cppClassName }}*>((({{cType}}_extended *)baton->self)->payload);
            {% else %}
              Configurable{{ cppClassName }}* instance = static_cast<Configurable{{ cppClassName }}*>(baton->{% each field.args|argsInfo as arg %}
              {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
              {% endeach %});
            {% endif %}
            baton->SetCallbackError(result);
            baton->result = {{ field.return.error }};
          }
          baton->Done();
        {% endif %}
      }
    {% endif %}
  {% endif %}
{% endeach %}
