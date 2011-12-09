/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
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
    static Persistent<FunctionTemplate> constructor_template;

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
    /**
     * Lookup a tree object from a repository.
     *
     * @param repo the repo to use when locating the tree.
     * @param id identity of the tree to locate.
     *
     * @return 0 on success; error code otherwise
     */
    int Lookup(git_repository* repo, const git_oid* id);
    /**
     * Get number of entries in the looked up tree.
     *
     * @return number of entries
     */
    size_t EntryCount();
    /**
     * Get entry by index in the looked up tree.
     *
     * @param idx index of the entry
     *
     * @return git tree entry
     */
    git_tree_entry* EntryByIndex(int idx);

    git_tree_entry* EntryByName(const char* name);

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

    static Handle<Value> Lookup(const Arguments& args);
    static int EIO_Lookup(eio_req *req);
    static int EIO_AfterLookup(eio_req *req);
    static Handle<Value> EntryCount(const Arguments& args);
    static Handle<Value> EntryByIndex(const Arguments& args);
    static void EIO_EntryByIndex(eio_req *req);
    static int EIO_AfterEntryByIndex(eio_req *req);
    static Handle<Value> EntryByName(const Arguments& args);
    static void EIO_EntryByName(eio_req *req);
    static int EIO_AfterEntryByName(eio_req *req);
    static Handle<Value> SortEntries(const Arguments& args);
    static Handle<Value> ClearEntries(const Arguments& args);

  private:
    /**
     * Internal reference to git_tree object
     */
    git_tree* tree;
    /**
     * Structure to handle async lookups
     */
    struct lookup_request {
      GitTree* tree;
      GitRepo* repo;
      GitOid* oid;
      int err;
      Persistent<Function> callback;
    };
    /**
     * Structure to handle async entryByIndex
     */
    struct entryindex_request {
      GitTree* tree;
      GitTreeEntry* entry;
      int idx;
      Persistent<Function> callback;
    };
    /**
     * Structure to handle async entryByName
     */
    struct entryname_request {
      GitTree* tree;
      GitTreeEntry* entry;
      std::string name;
      Persistent<Function> callback;
    };
};

#endif
