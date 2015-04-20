{%each args as cbFunction %}
  {%if cbFunction.isCallbackFunction %}

{{ cbFunction.return.type }} {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_cppCallback (
  {% each cbFunction.args|argsInfo as arg %}
    {{ arg.cType }} {{ arg.name}}{% if not arg.lastArg %},{% endif %}
  {% endeach %}
) {
  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = new {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton();

  {% each cbFunction.args|argsInfo as arg %}
    baton->{{ arg.name }} = {{ arg.name }};
  {% endeach %}

  baton->result = 0;
  baton->req.data = baton;
  baton->done = false;

  uv_async_init(uv_default_loop(), &baton->req, (uv_async_cb) {{ cppFunctionName }}_{{ cbFunction.name }}_async);
  uv_async_send(&baton->req);

  while(!baton->done) {
    this_thread::sleep_for(chrono::milliseconds(1));
  }

  {% each cbFunction|returnsInfo false true as _return %}
    {% if _return.isOutParam %}
    *{{ _return.name }} = *baton->{{ _return.name }};
    {% endif %}
  {% endeach %}

  return baton->result;
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_async(uv_async_t* req, int status) {
  NanScope();

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(req->data);

  {% each cbFunction.args|argsInfo as arg %}
    {% if arg | isPayload %}
      {% if cbFunction.payload.globalPayload %}
  NanCallback* callback = (({{ cppFunctionName }}_globalPayload*)baton->{{ arg.name }})->{{ cbFunction.name }};
      {% else %}
  NanCallback* callback = (NanCallback *)baton->{{ arg.name }};
      {% endif %}
    {% endif %}
  {% endeach %}

  Local<Value> argv[{{ cbFunction.args|jsArgsCount }}] = {
    {% each cbFunction.args|argsInfo as arg %}
      {% if arg | isPayload %}
        {%-- payload is always the last arg --%}
        // payload is null because we can use closure scope in javascript
        NanUndefined()
      {% elsif arg.isJsArg %}
        {% if arg.isEnum %}
          NanNew((int)baton->{{ arg.name }}),
        {% elsif arg.isLibgitType %}
          NanNew({{ arg.cppClassName }}::New((void *)baton->{{ arg.name }}, false)),
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
  Handle<v8::Value> result = callback->Call({{ cbFunction.args|jsArgsCount }}, argv);

  if (result->IsObject() && result->ToObject()->Has(NanNew("then"))) {
    Handle<v8::Value> thenProp = result->ToObject()->Get(NanNew("then"));

    if (thenProp->IsFunction()) {
      // we can be reasonbly certain that the result is a promise
      Local<Object> promise = result->ToObject();

      NanAssignPersistent(baton->promise, promise);

      uv_close((uv_handle_t*) &baton->req, NULL);
      uv_async_init(uv_default_loop(), &baton->req, (uv_async_cb) {{ cppFunctionName }}_{{ cbFunction.name }}_asyncPromisePolling);
      uv_async_send(&baton->req);
      return;
    }
  }

  {% each cbFunction|returnsInfo false true as _return %}
    if (result.IsEmpty() || result->IsNativeError()) {
      baton->result = {{ cbFunction.return.error }};
    }
    else if (!result->IsNull() && !result->IsUndefined()) {
      {% if _return.isOutParam %}
      {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
      wrapper->selfFreeing = false;

      baton->{{ _return.name }} = wrapper->GetRefValue();
      baton->result = {{ cbFunction.return.success }};
      {% else %}
      if (result->IsNumber()) {
        baton->result = (int)result->ToNumber()->Value();
      }
      else {
        baton->result = {{ cbFunction.return.noResults }};
      }
      {% endif %}
    }
    else {
      baton->result = {{ cbFunction.return.noResults }};
    }
  {% endeach %}

  baton->done = true;
  uv_close((uv_handle_t*) &baton->req, NULL);
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_asyncPromisePolling(uv_async_t* req, int status) {
  NanScope();

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(req->data);
  Local<Object> promise = NanNew<Object>(baton->promise);
  NanCallback* isPendingFn = new NanCallback(promise->Get(NanNew("isPending")).As<Function>());
  Local<Value> argv[1]; // MSBUILD won't assign an array of length 0
  Local<Boolean> isPending = isPendingFn->Call(promise, 0, argv)->ToBoolean();

  if (isPending->Value()) {
    uv_async_send(&baton->req);
    return;
  }

  NanCallback* isFulfilledFn = new NanCallback(promise->Get(NanNew("isFulfilled")).As<Function>());
  Local<Boolean> isFulfilled = isFulfilledFn->Call(promise, 0, argv)->ToBoolean();

  if (isFulfilled->Value()) {
    NanCallback* resultFn = new NanCallback(promise->Get(NanNew("value")).As<Function>());
    Handle<v8::Value> result = resultFn->Call(promise, 0, argv);

    {% each cbFunction|returnsInfo false true as _return %}
      if (result.IsEmpty() || result->IsNativeError()) {
        baton->result = {{ cbFunction.return.error }};
      }
      else if (!result->IsNull() && !result->IsUndefined()) {
        {% if _return.isOutParam %}
        {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
        wrapper->selfFreeing = false;

        baton->{{ _return.name }} = wrapper->GetRefValue();
        baton->result = {{ cbFunction.return.success }};
        {% else %}
        if (result->IsNumber()) {
          baton->result = (int)result->ToNumber()->Value();
        }
        else {
          baton->result = {{ cbFunction.return.noResults }};
        }
        {% endif %}
      }
      else {
        baton->result = {{ cbFunction.return.noResults }};
      }
    {% endeach %}
    baton->done = true;
  }
  else {
    // promise was rejected
    baton->result = {{ cbFunction.return.error }};
    baton->done = false;
  }

  uv_close((uv_handle_t*) &baton->req, NULL);
}
  {%endif%}
{%endeach%}
