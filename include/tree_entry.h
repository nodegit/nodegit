/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
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
    static Persistent<FunctionTemplate> constructor_template;

    /**
     * Used to intialize the EventEmitter from Node.js
     *
     * @param target v8::Object the Node.js module object
     */
    static void Initialize(Handle<v8::Object> target);

    /**
     * Accessor for GitTreeEntry
     *
     * @return the internal git_tree_entry reference
     */
    git_tree_entry* GetValue();

    /**
     * Mutator for GitTreeEntry
     *
     * @param obj a git_tree_entry object
     */
    void SetValue(git_tree_entry* tree);
    const char* Name();
    int Attributes();
    const git_oid* Id();
    int ToObject(git_repository* repo, git_object** obj);

  protected:
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Attributes(const Arguments& args);
    static Handle<Value> Id(const Arguments& args);
    static Handle<Value> ToObject(const Arguments& args);

  private:
    git_tree_entry* entry;
};

#endif
