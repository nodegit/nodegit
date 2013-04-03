/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef OID_H
#define OID_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitOid : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;

    static void Initialize (Handle<v8::Object> target);
    git_oid GetValue();
    void SetValue(git_oid oid);

  protected:
    GitOid() {}
    ~GitOid() {}

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Sha(const Arguments& args);

    static Handle<Value> FromString(const Arguments& args);
    static void FromStringWork(uv_work_t* req);
    static void FromStringAfterWork(uv_work_t* req);

  private:
    git_oid oid;

    struct FromStringBaton {
      uv_work_t request;
      const git_error* error;

      std::string fromString;
      GitOid* oid;
      git_oid rawOid;

      Persistent<Function> callback;
    };
};

#endif
