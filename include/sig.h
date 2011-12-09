/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef GitSig_H
#define GitSig_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"

using namespace v8;
using namespace node;

class GitSig : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    void New(const char *name, const char *email, time_t time, int offset);
    git_signature* GetValue();
    void SetValue(git_signature* GitSig);
    git_signature* Dup();
    void Free();

    char* Name();
    char* Email();

  protected:
    GitSig() {};
    ~GitSig() {};

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Dup(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Email(const Arguments& args);

  private:
    git_signature* sig;

    char* name;
    char* email;
};

#endif
