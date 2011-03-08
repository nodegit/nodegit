/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef BLOB_H
#define BLOB_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"

using namespace v8;
using namespace node;

/**
 * Class wrapper for libgit2 git_blob
 */
class Blob : public EventEmitter {
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
     * Creates new internal git_blob reference
     *
     * @param repo the repo to use when creating the blob.
     * @return 0 on success; error code otherwise
     */
    int New(git_repository *repo);

    /**
     * Accessor for Blob
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
     * Lookup a blob object from a repository.
     *
     * @param blob pointer to the looked up blob
     * @param repo the repo to use when locating the blob.
     * @param id identity of the blob to locate.
     *
     * @return 0 on success; error code otherwise
     */
    int Lookup(git_repository *repo, const git_oid *id);
    const char* RawContent();
    int RawSize();

  protected:
    /**
     * Constructor
     */
    Blob() {};

    /**
     * Deconstructor
     */
    ~Blob() {};

    /**
     * Creates a new instance of Blob to Node.js
     *
     * @param args v8::Arguments function call arguments from Node.js
     *
     * @return v8::Object args.This()
     */
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static int EIO_Lookup(eio_req *req);
    static int EIO_AfterLookup(eio_req *req);

    static Handle<Value> RawContent(const Arguments& args);
    static Handle<Value> RawSize(const Arguments& args);

  private:
    /**
     * Internal reference to git_blob object
     */
    git_blob *blob;

    struct lookup_request {
      Blob *blob;
      Repo *repo;
      Oid *oid;
      Persistent<Value> err;
      Persistent<Function> callback;
    };
};

#endif
