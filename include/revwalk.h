/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREVWALK_H
#define GITREVWALK_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitRevWalk : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_revwalk *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRevWalk(git_revwalk *raw);
    ~GitRevWalk();

    static NAN_METHOD(New);

    static NAN_METHOD(Reset);

    struct PushBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const git_oid * id;
    };
    class PushWorker : public NanAsyncWorker {
      public:
        PushWorker(
            PushBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~PushWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        PushBaton *baton;
    };
    static NAN_METHOD(Push);

    struct PushGlobBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const char * glob;
    };
    class PushGlobWorker : public NanAsyncWorker {
      public:
        PushGlobWorker(
            PushGlobBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~PushGlobWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        PushGlobBaton *baton;
    };
    static NAN_METHOD(PushGlob);

    struct PushHeadBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
    };
    class PushHeadWorker : public NanAsyncWorker {
      public:
        PushHeadWorker(
            PushHeadBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~PushHeadWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        PushHeadBaton *baton;
    };
    static NAN_METHOD(PushHead);

    struct HideBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const git_oid * commit_id;
    };
    class HideWorker : public NanAsyncWorker {
      public:
        HideWorker(
            HideBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~HideWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        HideBaton *baton;
    };
    static NAN_METHOD(Hide);

    struct HideGlobBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const char * glob;
    };
    class HideGlobWorker : public NanAsyncWorker {
      public:
        HideGlobWorker(
            HideGlobBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~HideGlobWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        HideGlobBaton *baton;
    };
    static NAN_METHOD(HideGlob);

    struct HideHeadBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
    };
    class HideHeadWorker : public NanAsyncWorker {
      public:
        HideHeadWorker(
            HideHeadBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~HideHeadWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        HideHeadBaton *baton;
    };
    static NAN_METHOD(HideHead);

    struct PushRefBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const char * refname;
    };
    class PushRefWorker : public NanAsyncWorker {
      public:
        PushRefWorker(
            PushRefBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~PushRefWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        PushRefBaton *baton;
    };
    static NAN_METHOD(PushRef);

    struct HideRefBaton {
      int error_code;
      const git_error* error;
      git_revwalk * walk;
      const char * refname;
    };
    class HideRefWorker : public NanAsyncWorker {
      public:
        HideRefWorker(
            HideRefBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~HideRefWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        HideRefBaton *baton;
    };
    static NAN_METHOD(HideRef);

    struct NextBaton {
      int error_code;
      const git_error* error;
      git_oid * out;
      git_revwalk * walk;
    };
    class NextWorker : public NanAsyncWorker {
      public:
        NextWorker(
            NextBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~NextWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        NextBaton *baton;
    };
    static NAN_METHOD(Next);
    static NAN_METHOD(Sorting);
    git_revwalk *raw;
};

#endif
