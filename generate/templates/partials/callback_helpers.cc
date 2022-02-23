{%each args as cbFunction %}
  {%if cbFunction.isCallbackFunction %}

{{ cbFunction.return.type }} {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_cppCallback (
  {% each cbFunction.args|argsInfo as arg %}
    {{ arg.cType }} {{ arg.name}}{% if not arg.lastArg %},{% endif %}
  {% endeach %}
) {
  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton baton({{ cbFunction.return.noResults }});

  {% each cbFunction.args|argsInfo as arg %}
    baton.{{ arg.name }} = {{ arg.name }};
  {% endeach %}

  return baton.ExecuteAsync({{ cppFunctionName }}_{{ cbFunction.name }}_async, {{ cppFunctionName }}_{{ cbFunction.name }}_cancelAsync);
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_cancelAsync(void *untypedBaton) {
  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(untypedBaton);
  baton->result = {{ cbFunction.return.cancel }};
  baton->Done();
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_async(void *untypedBaton) {
  Nan::HandleScope scope;

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(untypedBaton);

  {% each cbFunction.args|argsInfo as arg %}
    {% if arg | isPayload %}
      {% if cbFunction.payload.globalPayload %}
  Nan::Callback* callback = (({{ cppFunctionName }}_globalPayload*)baton->{{ arg.name }})->{{ cbFunction.name }};
      {% else %}
  Nan::Callback* callback = (Nan::Callback *)baton->{{ arg.name }};
      {% endif %}
    {% endif %}
  {% endeach %}

  v8::Local<v8::Value> argv[{{ cbFunction.args|callbackArgsCount }}] = {
    {% each cbFunction.args|callbackArgsInfo as arg %}
      {% if not arg.firstArg %}, {% endif %}
      {% if arg.isEnum %}
        Nan::New((int)baton->{{ arg.name }})
      {% elsif arg.isLibgitType %}
        {{ arg.cppClassName }}::New(baton->{{ arg.name }}, false)
      {% elsif arg.cType == "size_t" %}
        // HACK: NAN should really have an overload for Nan::New to support size_t
        Nan::New((unsigned int)baton->{{ arg.name }})
      {% elsif arg.cppClassName == 'String' %}
        Nan::New(baton->{{ arg.name }}).ToLocalChecked()
      {% else %}
        Nan::New(baton->{{ arg.name }})
      {% endif %}
    {% endeach %}
  };

  Nan::TryCatch tryCatch;
  Nan::MaybeLocal<v8::Value> maybeResult = (*callback)(baton->GetAsyncResource(), {{ cbFunction.args|callbackArgsCount }}, argv);

  v8::Local<v8::Value> result;
  if (!maybeResult.IsEmpty()) {
    result = maybeResult.ToLocalChecked();
  }

  if(PromiseCompletion::ForwardIfPromise(result, baton, {{ cppFunctionName }}_{{ cbFunction.name }}_promiseCompleted)) {
    return;
  }

  {% each cbFunction|returnsInfo false true as _return %}
    if (result.IsEmpty() || result->IsNativeError()) {
      baton->result = {{ cbFunction.return.error }};
    }
    else if (!result->IsNull() && !result->IsUndefined()) {
      {% if _return.isOutParam %}
      {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(Nan::To<v8::Object>(result).ToLocalChecked());
      wrapper->selfFreeing = false;

      *baton->{{ _return.name }} = wrapper->GetValue();
      baton->result = {{ cbFunction.return.success }};
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
}

void {{ cppClassName }}::{{ cppFunctionName }}_{{ cbFunction.name }}_promiseCompleted(bool isFulfilled, nodegit::AsyncBaton *_baton, v8::Local<v8::Value> result) {
  Nan::HandleScope scope;

  {{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton* baton = static_cast<{{ cppFunctionName }}_{{ cbFunction.name|titleCase }}Baton*>(_baton);

  if (isFulfilled) {
    {% each cbFunction|returnsInfo false true as _return %}
      if (result.IsEmpty() || result->IsNativeError()) {
        baton->result = {{ cbFunction.return.error }};
      }
      else if (!result->IsNull() && !result->IsUndefined()) {
        {% if _return.isOutParam %}
        {{ _return.cppClassName }}* wrapper = Nan::ObjectWrap::Unwrap<{{ _return.cppClassName }}>(Nan::To<v8::Object>(result).ToLocalChecked());
        wrapper->selfFreeing = false;

        *baton->{{ _return.name }} = wrapper->GetValue();
        baton->result = {{ cbFunction.return.success }};
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
    {{ cppClassName }}* instance = static_cast<{{ cppClassName }}*>(baton->{% each cbFunction.args|argsInfo as arg %}
      {% if arg.payload == true %}{{arg.name}}{% elsif arg.lastArg %}{{arg.name}}{% endif %}
    {% endeach %});
    v8::Local<v8::Object> parent = instance->handle();
    SetPrivate(parent, Nan::New("NodeGitPromiseError").ToLocalChecked(), result);

    baton->result = {{ cbFunction.return.error }};
  }
  baton->Done();
}
  {%endif%}
{%endeach%}
