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
          {% if arg.cppClassName == "GitIndexerProgress" %}
            if ({{ arg.name }}) {
              baton->stats_copy = *{{ arg.name }};
              baton->{{ arg.name }} = &baton->stats_copy;
            } else {
              baton->{{ arg.name }} = nullptr;
            }
          {% else %}
            baton->{{ arg.name }} = {{ arg.name }};
          {% endif %}
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
        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(untypedBaton);
        Configurable{{ cppClassName }}* instance = {{ field.jsFunctionName }}_getInstanceFromBaton(baton);

        if (!instance || !instance->{{ field.jsFunctionName }}.GetCallback() || instance->{{ field.jsFunctionName }}.GetCallback()->IsEmpty()) {
          {% if field.return.type == "int" %}
            baton->result = baton->defaultResult; // no results acquired
          {% endif %}
          baton->Done();
          return;
        }

        Napi::FunctionReference *callback = instance->{{ field.jsFunctionName }}.GetCallback();
        Napi::Env env = callback->Env();
        Napi::HandleScope scope(env);

        {% each field.args|callbackArgsInfo as arg %}
        {% if arg.cppClassName == "Array" %}
          Napi::Array _{{arg.name}}_array = Napi::Array::New(env, baton->{{ arg.arrayLengthArgumentName }});
          for(uint32_t i = 0; i < _{{arg.name}}_array.Length(); i++) {
            _{{arg.name}}_array.Set(i, {{arg.arrayElementCppClassName}}::New(baton->{{arg.name}}[i], false));
          }
        {% endif %}
        {% endeach %}

        {% if field.args|callbackArgsCount == 0 %}
          Napi::Value *argv = nullptr;
        {% else %}
          Napi::Value argv[{{ field.args|callbackArgsCount }}] = {
            {% each field.args|callbackArgsInfo as arg %}
            {% if not arg.firstArg %},{% endif %}
            {% if arg.isEnum %}
              Napi::Number::New(env, (int)baton->{{ arg.name }})
            {% elsif arg.cppClassName == "Array" %}
              _{{arg.name}}_array
            {% elsif arg.isLibgitType %}
              {{ arg.cppClassName }}::New(baton->{{ arg.name }}, false)
            {% elsif arg.cType == "size_t" %}
              Napi::Number::New(env, (unsigned int)baton->{{ arg.name }})
            {% elsif arg.cppClassName == "String" %}
              baton->{{ arg.name }} == NULL
                ? env.Null()
                : Napi::String::New(env, {%if arg.cType | isDoublePointer %}*{% endif %}baton->{{ arg.name }})
            {% else %}
              nodegit::ToV8(env, baton->{{ arg.name }})
            {% endif %}
            {% endeach %}
          };
        {% endif %}

        std::vector<napi_value> args;
        args.reserve({{ field.args|callbackArgsCount }});
        for (size_t i = 0; i < {{ field.args|callbackArgsCount }}; ++i) {
          args.push_back(argv[i]);
        }
        napi_value recv = env.Global();
        Napi::Value result;
        try {
          result = instance->{{ field.jsFunctionName }}.GetCallback()->MakeCallback(recv, args, *baton->GetAsyncResource());
        } catch (const Napi::Error &e) {
          {% if field.return.type != "void" %}
            baton->result = {{ field.return.error }};
          {% endif %}
          baton->Done();
          return;
        }

        if (PromiseCompletion::ForwardIfPromise(result, baton, Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_promiseCompleted)) {
          return;
        }

        {% if field.return.type == "void" %}
          baton->Done();
        {% else %}
          {% each field|returnsInfo false true as _return %}
            if (env.IsExceptionPending() || nodegit::IsError(env, result)) {
              baton->result = {{ field.return.error }};
            }
            else if (!result.IsNull() && !result.IsUndefined()) {
              {% if _return.isOutParam %}
                {{ _return.cppClassName }}* wrapper = NodeGitWrapper<{{ _return.cppClassName }}Traits>::Unwrap<{{ _return.cppClassName }}>(result.As<Napi::Object>());
                wrapper->selfFreeing = false;

                {% if _return.cppClassName == "GitOid" %}
                  git_oid_cpy(baton->{{ _return.name }}, wrapper->GetValue());
                {% else %}
                  *baton->{{ _return.name }} = wrapper->GetValue();
                {% endif %}
                baton->result = {{ field.return.success }};
              {% else %}
                if (result.IsNumber()) {
                  baton->result = result.As<Napi::Number>().Int32Value();
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

      void Configurable{{ cppClassName }}::{{ field.jsFunctionName }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, Napi::Value result) {
        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(_baton);
        Napi::Env env = result.Env();
        Napi::HandleScope scope(env);
        {% if field.return.type == "void" %}
          baton->Done();
        {% else %}
          if (isFulfilled) {
            {% each field|returnsInfo false true as _return %}
              if (env.IsExceptionPending() || nodegit::IsError(env, result)) {
                baton->result = {{ field.return.error }};
              }
              else if (!result.IsNull() && !result.IsUndefined()) {
                {% if _return.isOutParam %}
                  {{ _return.cppClassName }}* wrapper = NodeGitWrapper<{{ _return.cppClassName }}Traits>::Unwrap<{{ _return.cppClassName }}>(result.As<Napi::Object>());
                  wrapper->selfFreeing = false;

                  {% if _return.cppClassName == "GitOid" %}
                    git_oid_cpy(baton->{{ _return.name }}, wrapper->GetValue());
                  {% else %}
                    *baton->{{ _return.name }} = wrapper->GetValue();
                  {% endif %}
                  baton->result = {{ field.return.success }};
                {% else %}
                  if (result.IsNumber()) {
                    baton->result = result.As<Napi::Number>().Int32Value();
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
