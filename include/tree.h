/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef GITTREE_H
#define GITTREE_H

#include <v8.h>
#include <node.h>
#include <string>
#include <vector>

#include "git2.h"

#include "repo.h"
#include "tree_entry.h"

using namespace v8;
using namespace node;

/**
 * Class wrapper for libgit2 git_tree
 */
class GitTree : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;

    static const int WALK_ENTRY_SEND_THRESHOLD = 10;

    static void Initialize(Handle<v8::Object> target);

    git_tree* GetValue();

  protected:
    GitTree(git_tree *raw);
    ~GitTree();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t* req);
    static void LookupAfterWork(uv_work_t* req);

    static Handle<Value> Walk(const Arguments& args);
    static void WalkWork(void* payload);
    static int WalkWorkEntry(const char *root, const git_tree_entry *entry, void *payload);
    static void WalkWorkSendEntry(uv_async_t *handle, int status /*UNUSED*/);
    static void WalkWorkSendEnd(uv_async_t *handle, int status /*UNUSED*/);

    static Handle<Value> EntryByPath(const Arguments& args);
    static void EntryByPathWork(uv_work_t *req);
    static void EntryByPathAfterWork(uv_work_t *req);

  private:

    git_tree* raw;

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;

      git_oid rawOid;
      git_repository* rawRepo;
      git_tree* rawTree;

      Persistent<Function> callback;
    };

    struct WalkEntry {
        git_tree_entry* rawEntry;
        std::string root;
    };

    struct WalkBaton {
      uv_thread_t threadId;
      uv_mutex_t mutex;
      uv_async_t asyncEntry;
      uv_async_t asyncEnd;

      const git_error* error;

      std::vector<WalkEntry* > rawTreeEntries;

      git_tree* rawTree;
      bool blobsOnly;

      Persistent<Function> entryCallback;
      Persistent<Function> endCallback;
    };

    struct EntryByPathBaton {
      uv_work_t request;
      const git_error* error;

      git_tree* rawTree;
      std::string path;
      git_tree_entry* rawEntry;

      Persistent<Function> callback;
    };
};

#endif
