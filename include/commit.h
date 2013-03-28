/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <vector>

#include "../vendor/libgit2/include/git2.h"

#include "reference.h"
#include "repo.h"
#include "oid.h"
#include "tree.h"

#include "../include/functions/string.h"

using namespace node;
using namespace v8;

/**
 * Class wrapper for libgit2 git_commit
 */
class GitCommit : public ObjectWrap {
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
    static void Initialize (Handle<v8::Object> target);

    git_commit* GetValue();
    void SetValue(git_commit* commit);

    void Close();

  protected:
    GitCommit() {}
    ~GitCommit() {}

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Close(const Arguments& args);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t *req);
    static void LookupAfterWork(uv_work_t *req);

    static Handle<Value> Oid(const Arguments& args);

    static Handle<Value> Tree(const Arguments& args);
    static void TreeWork(uv_work_t* req);
    static void TreeAfterWork(uv_work_t* req);

    static Handle<Value> Parent(const Arguments& args);
    static void ParentWork(uv_work_t* req);
    static void ParentAfterWork(uv_work_t* req);

  private:
    git_commit* commit;
    git_oid* oid;

    struct LookupBaton {
      uv_work_t request;
      const git_error* error;

      git_repository* repo;
      git_oid rawOid;
      std::string sha;
      git_commit* rawCommit;

      Persistent<Function> callback;
    };

    struct TreeBaton {
      uv_work_t request;
      const git_error* error;

      git_commit* rawCommit;
      git_tree* rawTree;

      Persistent<Function> callback;
    };

    /**
     * Contains references to the current commit, parent commit (output)
     * parent commit's index, also contains references to an error code post
     * lookup, and a callback function to execute.
     */
    struct ParentBaton {
      uv_work_t request;
      const git_error* error;

      int index;
      git_commit* rawCommit;
      git_commit* rawParentCommit;

      Persistent<Function> callback;
    };

};
