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
#include <string>

#include "git2.h"

#include "repo.h"
#include "tree.h"
#include "oid.h"

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
    void SetRoot(std::string root);
    std::string GetRoot();

  protected:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Root(const Arguments& args);

    static Handle<Value> Name(const Arguments& args);
    static void NameWork(uv_work_t* req);
    static void NameAfterWork(uv_work_t* req);

    static Handle<Value> FileMode(const Arguments& args);
    static void FileModeWork(uv_work_t* req);
    static void FileModeAfterWork(uv_work_t* req);

    static Handle<Value> Oid(const Arguments& args);
    static void OidWork(uv_work_t* req);
    static void OidAfterWork(uv_work_t* req);

    static Handle<Value> ToBlob(const Arguments& args);
    static void ToBlobWork(uv_work_t *req);
    static void ToBlobAfterWork(uv_work_t *req);

  private:
    git_tree_entry* entry;
    std::string root;

    struct NameBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      const char* name;

      Persistent<Function> callback;
    };

    struct FileModeBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      git_filemode_t fileMode;

      Persistent<Function> callback;
    };

    struct OidBaton {
      uv_work_t request;

      git_tree_entry* rawEntry;
      const git_oid* rawOid;

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
