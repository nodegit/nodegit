// This is a generated file, modify: generate/templates/nodegit.cc.
#include <v8.h>
#include <node.h>
#include <git2.h>

#include "../include/wrapper.h"
#include "../include/functions/copy.h"
{%each%}
#include "../include/{{ filename }}"
{%endeach%}

extern "C" void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::Initialize(target);
  {%each%}
  {{ cppClassName }}::Initialize(target);
  {%endeach%}
}

NODE_MODULE(nodegit, init)
