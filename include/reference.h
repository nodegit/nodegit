/**
 * Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef REF_H
#define REF_H

#include <v8.h>
#include <node.h>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "oid.h"

using namespace node;
using namespace v8;

class GitReference : public ObjectWrap {
  public:
    const git_oid* Oid();
    static Persistent<Function> constructor_template;
    static void Initialize(Handle<v8::Object> target);

    git_reference* GetValue();
    void SetValue(git_reference* ref);

  protected:
    GitReference() {}
    ~GitReference() {}
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Oid(const Arguments& args);
    static void OidWork(uv_work_t* req);
    static void OidAfterWork(uv_work_t* req);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);


  private:
    git_reference *ref;

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;

      GitReference* ref;
      git_reference* rawRef;
      git_repository* rawRepo;
      std::string name;

      Persistent<Function> callback;
    };

    struct OidBaton {
      uv_work_t request;
      const git_error* error;

      const git_oid* rawOid;
      git_reference* rawRef;

      Persistent<Function> callback;
    };
};

#endif
