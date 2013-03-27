/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef GITTREEENTRY_H
#define GITTREEENTRY_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "tree.h"
#include "oid.h"
#include "object.h"

using namespace v8;
using namespace node;

/**
 * Class wrapper for libgit2 git_tree_entry
 */
class GitTreeEntry : ObjectWrap {
  public:

    static Persistent<Function> constructor_template;

    static void Initialize(Handle<v8::Object> target);
    git_tree_entry* GetValue();
    void SetValue(git_tree_entry* tree);

  protected:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Name(const Arguments& args);
    static void NameWork(uv_work_t* req);
    static void NameAfterWork(uv_work_t* req);

    static Handle<Value> FileMode(const Arguments& args);
    static void FileModeWork(uv_work_t* req);
    static void FileModeAfterWork(uv_work_t* req);

    static Handle<Value> Id(const Arguments& args);
    static void IdWork(uv_work_t* req);
    static void IdAfterWork(uv_work_t* req);

    static Handle<Value> ToBlob(const Arguments& args);
    static void ToBlobWork(uv_work_t *req);
    static void ToBlobAfterWork(uv_work_t *req);

  private:
    git_tree_entry* entry;

    struct NameBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      std::string name;

      Persistent<Function> callback;
    };

    struct FileModeBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      int fileMode;

      Persistent<Function> callback;
    };

    struct IdBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      git_oid* rawOid;

      Persistent<Function> callback;
    };

    struct ToBlobBaton {
      uv_work_t request;
      const git_error* error;

      git_repository* rawRepo;
      git_tree_entry* rawEntry;
      git_blob* rawBlob;

      Persistent<Function> callback;
    };
};

#endif
