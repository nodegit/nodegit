/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef OID_H
#define OID_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

using namespace node;
using namespace v8;

class GitOid : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);
    Handle<Value> WrapObj(Local<Object> obj);
    git_oid GetValue();
    void SetValue(git_oid oid);

    int Mkstr(const char* str);
    void Mkraw(const unsigned char* raw);
    void Fmt(char* buffer);
    void PathFmt(char *str);
    char* AllocFmt();
    char* ToString(char* buffer, size_t bufferSize);
    void Cpy(git_oid* out);
    int Cmp(const git_oid* a, const git_oid* b);

    GitOid() {}
    ~GitOid() {}

  protected:
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Mkstr(const Arguments& args);
    static Handle<Value> Mkraw(const Arguments& args);
    static Handle<Value> Fmt(const Arguments& args);
    static Handle<Value> PathFmt(const Arguments& args);
    static Handle<Value> AllocFmt(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);
    static Handle<Value> Cpy(const Arguments& args);
    static Handle<Value> Cmp(const Arguments& args);

  private:
    git_oid oid;
};

#endif
