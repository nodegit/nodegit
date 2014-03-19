/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITCLONEOPTIONS_H
#define GITCLONEOPTIONS_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitCloneOptions : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_clone_options *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitCloneOptions(git_clone_options *raw);
    ~GitCloneOptions();

    static NAN_METHOD(New);

    git_clone_options *raw;
};

#endif
