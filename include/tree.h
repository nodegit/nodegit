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

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"
#include "tree_entry.h"

using namespace v8;
using namespace node;

/**
 * Class wrapper for libgit2 git_tree
 */
class GitTree : public ObjectWrap {
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
    /**
     * Accessor for GitTree
     *
     * @return the internal git_tree reference
     */
    git_tree* GetValue();
    /**
     * Mutator for GitTree
     *
     * @param obj a git_tree object
     */
    void SetValue(git_tree* tree);

  protected:
    GitTree() {};
    ~GitTree() {};

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> EntryByPath(const Arguments& args);
    static void EntryByPathWork(uv_work_t *req);
    static void EntryByPathAfterWork(uv_work_t *req);

  private:
    /**
     * Internal reference to git_tree object
     */
    git_tree* tree;

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
