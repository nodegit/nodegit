{% each fields|fieldsInfo as field %}
  {% if not field.ignore %}
    NAN_GETTER({{ cppClassName }}::Get{{ field.cppFunctionName }}) {
      NanScope();

      {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

      {% if field.isEnum %}
        NanReturnValue(NanNew((int)wrapper->GetValue()->{{ field.name }}));

      {% elsif field.isLibgitType | or field.payloadFor %}
        NanReturnValue(wrapper->{{ field.name }});

      {% elsif field.isCallbackFunction %}
        NanReturnValue(wrapper->{{ field.name }}->GetFunction());

      {% elsif field.cppClassName == 'String' %}
        if (wrapper->GetValue()->{{ field.name }}) {
          NanReturnValue(NanNew<String>(wrapper->GetValue()->{{ field.name }}));
        }
        else {
          NanReturnUndefined();
        }

      {% elsif field.cppClassName|isV8Value %}
        NanReturnValue(NanNew<{{ field.cppClassName }}>(wrapper->GetValue()->{{ field.name }}));
      {% endif %}
    }

    NAN_SETTER({{ cppClassName }}::Set{{ field.cppFunctionName }}) {
      NanScope();

      {{ cppClassName }} *wrapper = ObjectWrap::Unwrap<{{ cppClassName }}>(args.This());

      {% if field.isEnum %}
        if (value->IsNumber()) {
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value->Int32Value();
        }

      {% elsif field.isLibgitType %}
        Handle<Object> {{ field.name }}(value->ToObject());
        NanDisposePersistent(wrapper->{{ field.name }});

        NanAssignPersistent(wrapper->{{ field.name }}, {{ field.name }});

        wrapper->raw->{{ field.name }} = {% if not field.cType | isPointer %}*{% endif %}ObjectWrap::Unwrap<{{ field.cppClassName }}>({{ field.name }}->ToObject())->GetValue();

      {% elsif field.isCallbackFunction %}
        if (wrapper->{{ field.name }} != NULL) {
          delete wrapper->{{ field.name }};
        }

        if (value->IsFunction()) {
          if (!wrapper->raw->{{ field.name }}) {
            wrapper->raw->{{ field.name }} = ({{ field.cType }}){{ field.name }}_cppCallback;
          }

          wrapper->{{ field.name }} = new NanCallback(value.As<Function>());
        }

      {% elsif field.payloadFor %}
        NanAssignPersistent(wrapper->{{ field.name }}, value);

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
          wrapper->GetValue()->{{ field.name }} = ({{ field.cType }}) value->Int32Value();
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

        uv_queue_work(uv_default_loop(), &baton->req, {{ field.name }}_asyncWork, {{ field.name }}_asyncAfter);

        while(!baton->done) {
          this_thread::sleep_for(chrono::milliseconds(1));
        }

        {% each field|returnsInfo false true as _return %}
          {% if _return.isOutParam %}
          *{{ _return.name }} = *baton->{{ _return.name }};
          {% endif %}
        {% endeach %}

        return baton->result;
      }

      void {{ cppClassName }}::{{ field.name }}_asyncWork(uv_work_t* req) {
        // We aren't doing any work on a seperate thread, just need to
        // access the main node thread in the async after method.
        // However, this worker method is still needed
      }

      void {{ cppClassName }}::{{ field.name }}_asyncAfter(uv_work_t* req, int status) {
        NanScope();

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(req->data);
        {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->payload);

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
          if (baton->{{ arg.name }} == NULL) {
            {% if arg.cType == "const char *" %}
              baton->{{ arg.name }} = "";
            {% elsif arg.cType == "unsigned int" %}
              baton->{{ arg.name }} = 0;
            {% endif %}
          }
          {% endif %}
        {% endeach %}

        Local<Value> argv[{{ field.args|jsArgsCount }}] = {
          {% each field.args|argsInfo as arg %}
            {% if arg.name == "payload" %}
              {%-- payload is always the last arg --%}
              NanNew(instance->{{ fields|payloadFor field.name }})
            {% elsif arg.isJsArg %}
              {% if arg.isEnum %}
                NanNew((int)baton->{{ arg.name }}),
              {% elsif arg.isLibgitType %}
                NanNew({{ arg.cppClassName }}::New(&baton->{{ arg.name }}, false)),
              {% elsif arg.cType == "size_t" %}
                // HACK: NAN should really have an overload for NanNew to support size_t
                NanNew((unsigned int)baton->{{ arg.name }}),
              {% else %}
                NanNew(baton->{{ arg.name }}),
              {% endif %}
            {% endif %}
          {% endeach %}
        };

        TryCatch tryCatch;
        Handle<v8::Value> result = instance->{{ field.name }}->Call({{ field.args|jsArgsCount }}, argv);

        if (result->IsObject() && result->ToObject()->Has(NanNew("then"))) {
          Handle<v8::Value> thenProp = result->ToObject()->Get(NanNew("then"));

          if (thenProp->IsFunction()) {
            // we can be reasonbly certain that the result is a promise
            Local<Object> promise = result->ToObject();

            NanAssignPersistent(baton->promise, promise);

            uv_queue_work(uv_default_loop(), &baton->req, {{ field.name }}_asyncWork, {{ field.name }}_asyncPromisePolling);
            return;
          }
        }

        {% each field|returnsInfo false true as _return %}
          if (result.IsEmpty() || result->IsNativeError()) {
            baton->result = {{ field.return.error }};
          }
          else if (!result->IsNull() && !result->IsUndefined()) {
            {% if _return.isOutParam %}
            {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
            wrapper->selfFreeing = false;

            baton->{{ _return.name }} = wrapper->GetRefValue();
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

      void {{ cppClassName }}::{{ field.name }}_asyncPromisePolling(uv_work_t* req, int status) {
        NanScope();

        {{ field.name|titleCase }}Baton* baton = static_cast<{{ field.name|titleCase }}Baton*>(req->data);
        Local<Object> promise = NanNew<Object>(baton->promise);
        NanCallback* isPendingFn = new NanCallback(promise->Get(NanNew("isPending")).As<Function>());
        Local<Value> argv[1]; // MSBUILD won't assign an array of length 0
        Local<Boolean> isPending = isPendingFn->Call(0, argv)->ToBoolean();

        if (isPending->Value()) {
          uv_queue_work(uv_default_loop(), &baton->req, {{ field.name }}_asyncWork, {{ field.name }}_asyncPromisePolling);
          return;
        }

        NanCallback* isFulfilledFn = new NanCallback(promise->Get(NanNew("isFulfilled")).As<Function>());
        Local<Boolean> isFulfilled = isFulfilledFn->Call(0, argv)->ToBoolean();

        if (isFulfilled->Value()) {
          NanCallback* resultFn = new NanCallback(promise->Get(NanNew("value")).As<Function>());
          Handle<v8::Value> result = resultFn->Call(0, argv);

          {% each field|returnsInfo false true as _return %}
            if (result.IsEmpty() || result->IsNativeError()) {
              baton->result = {{ field.return.error }};
            }
            else if (!result->IsNull() && !result->IsUndefined()) {
              {% if _return.isOutParam %}
              {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
              wrapper->selfFreeing = false;

              baton->{{ _return.name }} = wrapper->GetRefValue();
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
          baton->done = true;
        }
        else {
          // promise was rejected
          baton->result = {{ field.return.error }};
          baton->done = false;
        }
      }
    {% endif %}
  {% endif %}
{% endeach %}
