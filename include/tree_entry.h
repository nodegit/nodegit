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
    /**
     * v8::FunctionTemplate used to create Node.js constructor
     */
    static Persistent<Function> constructor_template;

    /**
     * Used to intialize the EventEmitter from Node.js
     *
     * @param target v8::Object the Node.js module object
     */
    static void Initialize(Handle<v8::Object> target);
    git_tree_entry* GetValue();
    void SetValue(git_tree_entry* tree);

  protected:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> ToBlob(const Arguments& args);
    static void ToBlobWork(uv_work_t *req);
    static void ToBlobAfterWork(uv_work_t *req);

  private:
    git_tree_entry* entry;

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
