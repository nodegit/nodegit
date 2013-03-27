/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef BLOB_H
#define BLOB_H

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "repo.h"

using namespace node;
using namespace v8;

/**
 * Class: GitBlob
 *   Wrapper for libgit2 git_blob.
 */
class GitBlob : public ObjectWrap {
  public:
    /**
     * Variable: constructor_template
     *   Used to create Node.js constructor.
     */
    static Persistent<Function> constructor_template;
    /**
     * Function: Initialize
     *   Used to intialize the EventEmitter from Node.js
     *
     * Parameters:
     *   target - Object the Node.js global module object
     */
    static void Initialize(Handle<Object> target);
    /**
     * Accessor for GitBlob
     *
     * @return the internal git_blob reference
     */
    git_blob* GetValue();
    /**
     * Mutator for Object
     *
     * @param obj a git_object object
     */
    void SetValue(git_blob* blob);
    /**
     * Function: Lookup
     *   Lookup a blob object from a repository.
     *
     * Parameters:
     *   repo the repo to use when locating the blob.
     *   id identity of the blob to locate.
     *
     * Returns:
     *   0 on success; error code otherwise
     */
    int Lookup(git_repository* repo, const git_oid *id);
    /**
     * Function: RawContent
     *   Get a read-only buffer with the raw content of a blob.
     *
     * Returns:
     *   raw content buffer; NULL if the blob has no contents
     */
    const void* RawContent();
    /**
     * Function: RawSize
     *   Lookup a blob object from a repository.
     *
     * Returns:
     *   size in bytes
     */
    int RawSize();
    /**
     *
     * Function: Close
     *   Free a blob object.
     */
    void Close();
    /**
     *
     * Function: CreateFromFile
     *   Read a file into the ODB.
     *
     *   Returns:
     *     0 on success, error code otherwise
     */
    int CreateFromFile(git_oid* oid, git_repository* repo, const char* path);
    /**
     *
     * Function: CreateFromBuffer
     *   Read a buffer into the ODB.
     *
     *   Returns:
     *     0 on success, error code otherwise
     */
    int CreateFromBuffer(git_oid* oid, git_repository* repo, const void* buffer, size_t len);

  protected:
    /**
     * Constructor: GitBlob
     */
    GitBlob() {};
    /**
     * Deconstructor: GitBlob
     */
    ~GitBlob() {};
    /**
     * Function: New
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> New(const Arguments& args);
    /**
     * Function: Lookup
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> Lookup(const Arguments& args);
    /**
     * Function: EIO_Lookup
     *
     * Parameters:
     *   req - an uv_work_t pointer
     *
     */
    static void EIO_Lookup(uv_work_t* req);
    /**
     * Function: EIO_AfterLookup
     *
     * Parameters:
     *   req - an uv_work_t pointer
     */
    static void EIO_AfterLookup(uv_work_t* req);
    /**
     * Function: RawContent
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> RawContent(const Arguments& args);
    /**
     * Function: RawSize
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> RawSize(const Arguments& args);
    /**
     * Function: Close
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> Close(const Arguments& args);
    /**
     * Function: CreateFromFile
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> CreateFromFile(const Arguments& args);
    /**
     * Function: CreateFromBuffer
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> CreateFromBuffer(const Arguments& args);

  private:
    /**
     * Variable: blob
     *   Internal reference to git_blob object
     */
    git_blob* blob;

    /**
     * Struct: lookup_request
     *   Contains references to the current blob, repo, and oid for a
     *   commit lookup, also contains references to an error code post
     *   lookup, and a callback function to execute.
     */
    struct lookup_request {
      GitBlob* blob;
      GitRepo* repo;
      GitOid* oid;
      int err;
      Persistent<Function> callback;
    };
};

#endif
