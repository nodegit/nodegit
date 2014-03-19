/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREVWALK_H
#define GITREVWALK_H

#include <v8.h>
#include <node.h>
#include <string>

#include "nan.h"

#include "git2.h"

using namespace node;
using namespace v8;

class GitRevWalk : public ObjectWrap {
  public:

    static Persistent<FunctionTemplate> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_revwalk *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRevWalk(git_revwalk *raw);
    ~GitRevWalk();

    static NAN_METHOD(New);

    static NAN_METHOD(Reset);
    static NAN_METHOD(Push);

    static void PushWork(uv_work_t* req);
    static void PushAfterWork(uv_work_t* req);

    struct PushBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static NAN_METHOD(PushGlob);
    static void PushGlobWork(uv_work_t* req);
    static void PushGlobAfterWork(uv_work_t* req);

    struct PushGlobBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> globReference;
      const char * glob;
      Persistent<Function> callback;
    };
    static NAN_METHOD(PushHead);
    static void PushHeadWork(uv_work_t* req);
    static void PushHeadAfterWork(uv_work_t* req);

    struct PushHeadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Hide);
    static void HideWork(uv_work_t* req);
    static void HideAfterWork(uv_work_t* req);

    struct HideBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> commit_idReference;
      const git_oid * commit_id;
      Persistent<Function> callback;
    };
    static NAN_METHOD(HideGlob);
    static void HideGlobWork(uv_work_t* req);
    static void HideGlobAfterWork(uv_work_t* req);

    struct HideGlobBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> globReference;
      const char * glob;
      Persistent<Function> callback;
    };
    static NAN_METHOD(HideHead);
    static void HideHeadWork(uv_work_t* req);
    static void HideHeadAfterWork(uv_work_t* req);

    struct HideHeadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Function> callback;
    };
    static NAN_METHOD(PushRef);
    static void PushRefWork(uv_work_t* req);
    static void PushRefAfterWork(uv_work_t* req);

    struct PushRefBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> refnameReference;
      const char * refname;
      Persistent<Function> callback;
    };
    static NAN_METHOD(HideRef);
    static void HideRefWork(uv_work_t* req);
    static void HideRefAfterWork(uv_work_t* req);

    struct HideRefBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Value> refnameReference;
      const char * refname;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Next);
    static void NextWork(uv_work_t* req);
    static void NextAfterWork(uv_work_t* req);

    struct NextBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * out;
      Persistent<Value> walkReference;
      git_revwalk * walk;
      Persistent<Function> callback;
    };
    static NAN_METHOD(Sorting);
    git_revwalk *raw;
};

#endif
