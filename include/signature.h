/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef GitSignature_H
#define GitSignature_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"

using namespace v8;
using namespace node;

class GitSignature : public ObjectWrap {
  public:
    static Persistent<Function> constructor_template;

    static void Initialize(Handle<v8::Object> target);

    git_signature* GetValue();
    void SetValue(git_signature* signature);

  protected:
    GitSignature() {};
    ~GitSignature() {};

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Duplicate(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Email(const Arguments& args);

  private:
    git_signature* signature;
};

#endif
