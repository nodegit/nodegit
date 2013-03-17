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
#include <map>

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

    static const int WALK_DELTA_THRESHHOLD = 10;

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

    /**
     * Walk the current git_diff_list
     */
    static Handle<Value> Walk(const Arguments& args);
    static void WalkWork(void *payload);
    static int WalkWorkFile(const git_diff_delta *delta, float progress,
                             void *payload);
    static int WalkWorkHunk(const git_diff_delta *delta, const git_diff_range *range,
                             const char *header, size_t header_len, void *payload);
    static int WalkWorkData(const git_diff_delta *delta, /** delta that contains this data */
                             const git_diff_range *range, /** range of lines containing this data */
                             char line_origin,            /** git_diff_list_t value from above */
                             const char *content,         /** diff data - not NUL terminated */
                             size_t content_len,          /** number of bytes of diff data */
                             void *payload);              /** user reference data */
    /**
     * Called when WalkWorkFile reaches its cache
     * thresholds. Passes data back to main thread
     *
     * @param payload The WalkBaton
     */
    static void WalkWorkSendFile(uv_async_t *handle, int status /*UNUSED*/);
    static void WalkWorkSendHunk(uv_async_t *handle, int status /*UNUSED*/);
    static void WalkWorkSendData(uv_async_t *handle, int status /*UNUSED*/);
    static void WalkWorkSendEnd(uv_async_t *handle, int status /*UNUSED*/);

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

    struct DeltaContent {
      git_diff_range *range;
      char lineOrigin;
      size_t contentLength;
      std::string content;
    };

    struct Delta {
      git_diff_delta *raw;
      std::vector<DeltaContent* > contents;
    };

    struct WalkBaton {
      uv_thread_t threadId;
      uv_mutex_t mutex;
      uv_async_t asyncFile;
      uv_async_t asyncHunk;
      uv_async_t asyncData;
      uv_async_t asyncEnd;

      const git_error* error;

      std::map<std::string, Delta* > fileDeltas;

      git_diff_list* rawDiffList;
      Persistent<Function> fileCallback;
      Persistent<Function> hunkCallback;
      Persistent<Function> lineCallback;
      Persistent<Function> endCallback;
    };
};
