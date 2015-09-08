// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>

#include "../include/wrapper.h"
#include "../include/functions/copy.h"
{% each %}
  {% if type != "enum" %}
    #include "../include/{{ filename }}.h"
  {% endif %}
{% endeach %}

extern "C" void init(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Wrapper::InitializeComponent(target);
  {% each %}
    {% if type != "enum" %}
      {{ cppClassName }}::InitializeComponent(target);
    {% endif %}
  {% endeach %}
}

NODE_MODULE(nodegit, init)
