/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef OID_H
#define OID_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

using namespace node;
using namespace v8;

class Oid : public EventEmitter {
  public:
    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);
    git_oid* GetValue();
    int Mkstr(const char *str);
    void Mkraw(const unsigned char *raw);
    char* Fmt();
    //void pathfmt(char *str, const git_oid *oid)
    //char* allocfmt(const git_oid *oid)
    //char* to_string(char *out, size_t n, const git_oid *oid)
    //void cpy(git_oid *out, const git_oid *src)
    //int cmp(const git_oid *a, const git_oid *b)

  protected:
    Oid() {}
    ~Oid() {}
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Mkstr(const Arguments& args);
    static Handle<Value> Mkraw(const Arguments& args);
    static Handle<Value> Fmt(const Arguments& args);

  private:
    git_oid oid;
};

#endif
