{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {

      {{ cppClassName }} *wrapper = Nan::ObjectWrap::Unwrap<{{ cppClassName }}>(info.This());

      {% if field.isEnum %}
        info.GetReturnValue().Set(Nan::New((int)wrapper->GetValue()->{{ field.name }}));

      {% elsif field.isLibgitType | or field.payloadFor %}
        info.GetReturnValue().Set(Nan::New(wrapper->{{ field.name }}));

      {% elsif field.isCallbackFunction %}
        if (wrapper->{{field.name}} != NULL) {
          info.GetReturnValue().Set(wrapper->{{ field.name }}->GetFunction());
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
        Local<Object> {{ field.name }}(value->ToObject());

        wrapper->{{ field.name }}.Reset({{ field.name }});

        wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}{% if field.cppClassName == 'GitStrarray' %}StrArrayConverter::Convert({{ field.name }}->ToObject()){% else %}Nan::ObjectWrap::Unwrap<{{ field.cppClassName }}>({{ field.name }}->ToObject())->GetValue(){% endif %};

      {% elsif field.isCallbackFunction %}
        if (wrapper->{{ field.name }} != NULL) {
          delete wrapper->{{ field.name }};
        }

        if (value->IsFunction()) {
          if (!wrapper->raw->{{ field.name }}) {
            wrapper->raw->{{ field.name }} = ({{ field.cType }}){{ field.name }}_cppCallback;
          }

          wrapper->{{ field.name }} = new Nan::Callback(value.As<Function>());
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
      {{ field.return.type }} {{ cppClassName }}::{{ field.name }}_cppCallback (
        {% each field.args|argsInfo as arg %}
          {{ arg.cType }} {{ arg.name}}{% if not arg.lastArg %},{% endif %}
        {% endeach %}
      ) {
        {{ field.name|titleCase }}Baton* baton = new {{ field.name|titleCase }}Baton();

        {% each field.args|argsInfo as arg %}
          baton->{{ arg.name }} = {{ arg.name }};
        {% endeach %}

        baton->result = 0;
        baton->req.data = baton;
        baton->done = false;

        uv_async_init(uv_default_loop(), &baton->req, (uv_async_cb) {{ field.name }}_async);
        {
          LockMaster::TemporaryUnlock temporaryUnlock;

          uv_async_send(&baton->req);

          while(!baton->done) {
            sleep_for_ms(1);
          }
        }

        return baton->result;
      }

      void {{ cppClassName }}::{{ field.name }}_async(uv_async_t* req, int status) {
        Nan::HandleScope scope;

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(req->data);
        {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->{% each field.args|argsInfo as arg %}
          {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
        {% endeach %});

        if (instance->{{ field.name }}->IsEmpty()) {
          {% if field.return.type == "int" %}
            baton->result = {{ field.return.noResults }}; // no results acquired
          {% endif %}

          baton->done = true;
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
            {% endif %}
          {% endif %}
        {% endeach %}

        Local<Value> argv[{{ field.args|jsArgsCount }}] = {
          {% each field.args|argsInfo as arg %}
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
              {% elsif arg.cppClassName == 'String' %}
                Nan::New(baton->{{ arg.name }}).ToLocalChecked(),
              {% else %}
                Nan::New(baton->{{ arg.name }}),
              {% endif %}
            {% endif %}
          {% endeach %}
        };

        Nan::TryCatch tryCatch;
        Local<v8::Value> result = instance->{{ field.name }}->Call({{ field.args|jsArgsCount }}, argv);

        uv_close((uv_handle_t*) &baton->req, NULL);

        if(PromiseCompletion::ForwardIfPromise(result, baton, {{ cppClassName }}::{{ field.name }}_promiseCompleted)) {
          return;
        }

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
              baton->result = (int)result->ToNumber()->Value();
            }
            else {
              baton->result = {{ field.return.noResults }};
            }
            {% endif %}
          }
          else {
            baton->result = {{ field.return.noResults }};
          }
        {% endeach %}
        baton->done = true;
      }

      void {{ cppClassName }}::{{ field.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result) {
        Nan::HandleScope scope;

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(_baton);

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
                baton->result = (int)result->ToNumber()->Value();
              }
              else{
                baton->result = {{ field.return.noResults }};
              }
              {% endif %}
            }
            else {
              baton->result = {{ field.return.noResults }};
            }
          {% endeach %}
        }
        else {
          // promise was rejected
          {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->{% each field.args|argsInfo as arg %}
            {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
          {% endeach %});
          Local<v8::Object> parent = instance->handle();
          parent->SetHiddenValue(Nan::New("NodeGitPromiseError").ToLocalChecked(), result);

          baton->result = {{ field.return.error }};
        }
        baton->done = true;
      }
    {% endif %}
  {% endif %}
{% endeach %}
