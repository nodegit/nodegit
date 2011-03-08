/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef GITTREE_H
#define GITTREE_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"

using namespace v8;
using namespace node;

/**
 * Class wrapper for libgit2 git_tree
 */
class GitTree : public EventEmitter {
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
     * Creates new internal git_tree reference
     *
     * @param repo the repo to use when creating the tree.
     * @return 0 on success; error code otherwise
     */
    int New(git_repository* repo);

    /**
     * Accessor for GitTree
     *
     * @return the internal git_tree reference
     */
    git_tree* GetValue();

    /**
     * Mutator for Object
     *
     * @param obj a git_object object
     */
    void SetValue(git_tree* tree);

    /**
     * Lookup a tree object from a repository.
     *
     * @param tree pointer to the looked up tree
     * @param repo the repo to use when locating the tree.
     * @param id identity of the tree to locate.
     *
     * @return 0 on success; error code otherwise
     */
    int Lookup(git_tree** tree, git_repository* repo, const git_oid* id);

    /**
     * Get number of entries in the looked up tree.
     *
     * @return number of entries
     */
    size_t EntryCount();

    /**
     * Get entry by index in the looked up tree.
     *
     * @return git tree entry
     */
    //GitTree::Entry EntryByIndex();

    int SortEntries();
    void ClearEntries();



  protected:
    /**
     * Constructor
     */
    GitTree() {};

    /**
     * Deconstructor
     */
    ~GitTree() {};

    /**
     * Creates a new instance of GitTree to Node.js
     *
     * @param args v8::Arguments function call arguments from Node.js
     *
     * @return v8::Object args.This()
     */
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> EntryCount(const Arguments& args);
    static Handle<Value> EntryByIndex(const Arguments& args);
    static Handle<Value> SortEntries(const Arguments& args);
    static Handle<Value> ClearEntries(const Arguments& args);

    // Experimental
    class Entry : EventEmitter {
      private:
        git_tree_entry* entry;
    };

  private:
    /**
     * Internal reference to git_tree object
     */
    git_tree* tree;
};

#endif
