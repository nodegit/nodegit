/**
 * Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef BLOB_H
#define BLOB_H

#include <v8.h>
#include <node.h>
#include <string.h>

#include "git2.h"

#include "repo.h"

using namespace v8;
using namespace node;

/**
 *   Wrapper for libgit2 git_blob.
 */
class GitBlob : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;

    static void Initialize(Handle<Object> target);

    git_blob* GetValue();
    void SetValue(git_blob* blob);

  protected:
    GitBlob() {};
    ~GitBlob() {};

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    static Handle<Value> RawContent(const Arguments& args);
    static void RawContentWork(uv_work_t* req);
    static void RawContentAfterWork(uv_work_t* req);

    static Handle<Value> CreateFromFile(const Arguments& args);
    static Handle<Value> CreateFromBuffer(const Arguments& args);

  private:

    git_blob* blob;

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;

      GitBlob* blob;
      git_blob* rawBlob;
      git_repository* rawRepo;
      git_oid rawOid;

      Persistent<Function> callback;
    };

    struct RawContentBaton {
        uv_work_t request;

        GitBlob* blob;
        git_blob* rawBlob;
        std::string rawContent;
        int rawSize;

        Persistent<Function> callback;
    };
};

#endif
