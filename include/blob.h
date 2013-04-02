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
     * Function: LookupWork
     *
     * Parameters:
     *   req - an uv_work_t pointer
     *
     */
    static void LookupWork(uv_work_t* req);
    /**
     * Function: LookupAfterWork
     *
     * Parameters:
     *   req - an uv_work_t pointer
     */
    static void LookupAfterWork(uv_work_t* req);

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
    static void RawContentWork(uv_work_t* req);
    static void RawContentAfterWork(uv_work_t* req);

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
