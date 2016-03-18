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
  {
    LockMaster::TemporaryUnlock temporaryUnlock;

    uv_async_send(&baton->req);

    while(!baton->done) {
      sleep_for_ms(1);
    }
  }

  return baton->result;
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_async(uv_async_t* req, int status) {
  Nan::HandleScope scope;

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(req->data);

  {% each cbFunction.args|argsInfo as arg %}
    {% if arg | isPayload %}
      {% if cbFunction.payload.globalPayload %}
  Nan::Callback* callback = (({{ cppFunctionName }}_globalPayload*)baton->{{ arg.name }})->{{ cbFunction.name }};
      {% else %}
  Nan::Callback* callback = (Nan::Callback *)baton->{{ arg.name }};
      {% endif %}
    {% endif %}
  {% endeach %}

  Local<Value> argv[{{ cbFunction.args|jsArgsCount }}] = {
    {% each cbFunction.args|argsInfo as arg %}
      {% if arg | isPayload %}
        {%-- payload is always the last arg --%}
        // payload is null because we can use closure scope in javascript
        Nan::Undefined()
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
  Local<v8::Value> result = callback->Call({{ cbFunction.args|jsArgsCount }}, argv);

  uv_close((uv_handle_t*) &baton->req, NULL);

  if(PromiseCompletion::ForwardIfPromise(result, baton, {{ cppFunctionName }}_{{ cbFunction.name }}_promiseCompleted)) {
    return;
  }

  {% each cbFunction|returnsInfo false true as _return %}
    if (result.IsEmpty() || result->IsNativeError()) {
      baton->result = {{ cbFunction.return.error }};
    }
    else if (!result->IsNull() && !result->IsUndefined()) {
      {% if _return.isOutParam %}
      {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
      wrapper->selfFreeing = false;

      *baton->{{ _return.name }} = wrapper->GetValue();
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

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_promiseCompleted(bool isFulfilled, AsyncBaton *_baton, v8::Local<v8::Value> result) {
  Nan::HandleScope scope;

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(_baton);

  if (isFulfilled) {
    {% each cbFunction|returnsInfo false true as _return %}
      if (result.IsEmpty() || result->IsNativeError()) {
        baton->result = {{ cbFunction.return.error }};
      }
      else if (!result->IsNull() && !result->IsUndefined()) {
        {% if _return.isOutParam %}
        {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(result->ToObject());
        wrapper->selfFreeing = false;

        *baton->{{ _return.name }} = wrapper->GetValue();
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
  }
  else {
    // promise was rejected
    {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->{% each cbFunction.args|argsInfo as arg %}
      {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
    {% endeach %});
    Local<v8::Object> parent = instance->handle();
    parent->SetHiddenValue(Nan::New("NodeGitPromiseError").ToLocalChecked(), result);

    baton->result = {{ cbFunction.return.error }};
  }
  baton->done = true;
}
  {%endif%}
{%endeach%}
