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

    int Tree(git_tree** tree);

  protected:
    GitCommit() {}
    ~GitCommit() {}

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> FetchDetailsSync(const Arguments& args);
    static Handle<Value> FetchDetails(const Arguments& args);
    static void FetchDetailsWork(uv_work_t *req);
    static void FetchDetailsAfterWork(uv_work_t *req);

    static Handle<Value> Lookup(const Arguments& args);
    static void LookupWork(uv_work_t *req);
    static void LookupAfterWork(uv_work_t *req);

    static Handle<Value> Close(const Arguments& args);

    static Handle<Value> Tree(const Arguments& args);
    static void TreeWork(uv_work_t* req);
    static void TreeAfterWork(uv_work_t* req);

    static Handle<Value> ParentSync(const Arguments& args);
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
      git_oid oid;
      std::string sha;
      git_commit* rawCommit;

      Persistent<Function> callback;
    };

    /**
     * Struct containing details for a commit.
     */
    struct FetchDetailsBaton {
      uv_work_t request;
      const git_error* error;

      git_commit* rawCommit;
      const git_oid *oid;
      char sha[GIT_OID_HEXSZ + 1];
      const char* message;
      time_t time;
      int timeOffset;
      const git_signature* committer;
      const git_signature* author;
      unsigned int parentCount;
      std::vector<std::string> parentShas;

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
      GitCommit* commit;
      git_commit* rawParentCommit;

      Persistent<Function> callback;
    };

};
