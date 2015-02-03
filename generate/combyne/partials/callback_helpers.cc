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

  baton->req.data = baton;
  baton->done = false;

  uv_queue_work(uv_default_loop(), &baton->req, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncWork, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncAfter);

  while(!baton->done) {
    this_thread::sleep_for(chrono::milliseconds(1));
  }

  {% each cbFunction|returnsInfo true false as _return %}
    *{{ _return.name }} = *baton->{{ _return.name }};
  {% endeach %}

  return baton->result;
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_asyncWork(uv_work_t* req) {
  // We aren't doing any work on a seperate thread, just need to
  // access the main node thread in the async after method.
  // However, this worker method is still needed
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_asyncAfter(uv_work_t* req, int status) {
  NanScope();

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(req->data);

  CallbackWrapper* cbWrapper = (CallbackWrapper *)baton->payload;

  Local<Value> argv[{{ cbFunction.args|jsArgsCount }}] = {
    {% each cbFunction.args|argsInfo as arg %}
      {% if arg.name == "payload" %}
        {%-- payload is always the last arg --%}
        NanNew(cbWrapper->payload)
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
  Handle<Value> result = cbWrapper->jsCallback->Call({{ cbFunction.args|jsArgsCount }}, argv);

  if (result->IsObject() && result->ToObject()->Has(NanNew("then"))) {
    Handle<Value> thenProp = result->ToObject()->Get(NanNew("then"));

    if (thenProp->IsFunction()) {
      // we can be reasonbly certain that the result is a promise
      Local<Object> promise = result->ToObject();

      NanAssignPersistent(baton->promise, promise);

      uv_queue_work(uv_default_loop(), &baton->req, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncWork, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncPromisePolling);
      return;
    }
  }

  {{ cbFunction.return.type }} resultStatus;

  {% each cbFunction|returnsInfo true false as _return %}
    if (result.IsEmpty() || result->IsNativeError()) {
      baton->result = {{ cbFunction.return.error }};
    }
    else if (!result->IsNull() && !result->IsUndefined()) {
      {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
      wrapper->selfFreeing = false;

      baton->{{ _return.name }} = wrapper->GetRefValue();
      baton->result = {{ cbFunction.return.success }};
    }
    else {
      baton->result = {{ cbFunction.return.noResults }};
    }
  {% endeach %}
  baton->done = true;
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_asyncPromisePolling(uv_work_t* req, int status) {
  NanScope();

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(req->data);
  Local<Object> promise = NanNew<Object>(baton->promise);
  NanCallback* isPendingFn = new NanCallback(promise->Get(NanNew("isPending")).As<Function>());
  Local<Value> argv[1]; // MSBUILD won't assign an array of length 0
  Local<Boolean> isPending = isPendingFn->Call(0, argv)->ToBoolean();

  if (isPending->Value()) {
    uv_queue_work(uv_default_loop(), &baton->req, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncWork, {{ cppFunctionName }}_{{ cbFunction.name }}_asyncPromisePolling);
    return;
  }

  NanCallback* isFulfilledFn = new NanCallback(promise->Get(NanNew("isFulfilled")).As<Function>());
  Local<Boolean> isFulfilled = isFulfilledFn->Call(0, argv)->ToBoolean();

  if (isFulfilled->Value()) {
    NanCallback* resultFn = new NanCallback(promise->Get(NanNew("value")).As<Function>());
    Handle<Value> result = resultFn->Call(0, argv);
    {{ cbFunction.return.type }} resultStatus;

    {% each cbFunction|returnsInfo true false as _return %}
      if (result.IsEmpty() || result->IsNativeError()) {
        baton->result = {{ cbFunction.return.error }};
      }
      else if (!result->IsNull() && !result->IsUndefined()) {
        {{ _return.cppClassName }}* wrapper = ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
        wrapper->selfFreeing = false;

        baton->{{ _return.name }} = wrapper->GetRefValue();
        baton->result = {{ cbFunction.return.success }};
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
}
  {%endif%}
{%endeach%}
