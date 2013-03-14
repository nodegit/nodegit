/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>
#include <vector>
#include <string>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"

using namespace node;
using namespace v8;

/**
 * Class wrapper for libgit2 git_diff_list
 */
class GitDiffList : public ObjectWrap {
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

    git_diff_list* GetValue();
    void SetValue(git_diff_list* diffList);
    void Close();

  protected:
    GitDiffList() {}
    ~GitDiffList() {}

    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Close(const Arguments& args);

    static Handle<Value> TreeToTree(const Arguments& args);
    static void TreeToTreeWork(uv_work_t *req);
    static void TreeToTreeAfterWork(uv_work_t *req);

    static Handle<Value> Iterate(const Arguments& args);
    static void IterateWork(void *arg);

  private:
    git_diff_list* diffList;
    // git_oid* oldOid;
    // git_oid* newOid;

    struct TreeToTreeBaton {
      uv_work_t request;
      const git_error* error;

      GitDiffList *diffList;
      git_repository* repo;
      git_oid oldOid;
      std::string oldSha;
      git_oid newOid;
      std::string newSha;

      git_diff_list* rawDiffList;

      Persistent<Function> callback;
    };

    struct IterateBaton {
      uv_thread_t threadId;
      const git_error* error;

      git_diff_list* rawDiffList;
      Persistent<Function> fileCallback;
      Persistent<Function> hunkCallback;
      Persistent<Function> lineCallback;
      Persistent<Function> endCallback;
    };
};
