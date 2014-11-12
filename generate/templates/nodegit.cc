// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>

#include "../include/wrapper.h"
#include "../include/functions/copy.h"
{%each%}
#include "../include/{{ filename }}.h"
{%endeach%}

extern "C" void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::InitializeComponent(target);
  {%each%}
  {{ cppClassName }}::InitializeComponent(target);
  {%endeach%}
}

NODE_MODULE(nodegit, init)
