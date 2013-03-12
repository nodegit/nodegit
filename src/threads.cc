/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <string.h>
#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/threads.h"
#include "../include/error.h"

using namespace v8;
using namespace node;

void GitThreads::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("Threads"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "init", Init);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("Threads"), constructor_template);
}

Handle<Value> GitThreads::New(const Arguments& args) {
  HandleScope scope;

  GitThreads *threads = new GitThreads();

  threads->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitThreads::Init(const Arguments& args) {
  HandleScope scope;

  int returnCode = git_threads_init();
  if (returnCode) {
    return GitError::WrapError(giterr_last());
  }
  return True();
}

Persistent<Function> GitThreads::constructor_template;
