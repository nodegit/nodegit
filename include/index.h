/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITINDEX_H
#define GITINDEX_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitIndex : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_index *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitIndex(git_index *raw);
    ~GitIndex();

    static NAN_METHOD(New);


    struct OpenBaton {
      int error_code;
      const git_error* error;
      git_index * out;
      const char * index_path;
    };
    class OpenWorker : public NanAsyncWorker {
      public:
        OpenWorker(
            OpenBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~OpenWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        OpenBaton *baton;
    };
    static NAN_METHOD(Open);

    struct ReadBaton {
      int error_code;
      const git_error* error;
      git_index * index;
    };
    class ReadWorker : public NanAsyncWorker {
      public:
        ReadWorker(
            ReadBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~ReadWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        ReadBaton *baton;
    };
    static NAN_METHOD(Read);

    struct WriteBaton {
      int error_code;
      const git_error* error;
      git_index * index;
    };
    class WriteWorker : public NanAsyncWorker {
      public:
        WriteWorker(
            WriteBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~WriteWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        WriteBaton *baton;
    };
    static NAN_METHOD(Write);

    struct ReadTreeBaton {
      int error_code;
      const git_error* error;
      git_index * index;
      const git_tree * tree;
    };
    class ReadTreeWorker : public NanAsyncWorker {
      public:
        ReadTreeWorker(
            ReadTreeBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~ReadTreeWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        ReadTreeBaton *baton;
    };
    static NAN_METHOD(ReadTree);

    struct WriteTreeBaton {
      int error_code;
      const git_error* error;
      git_oid * out;
      git_index * index;
    };
    class WriteTreeWorker : public NanAsyncWorker {
      public:
        WriteTreeWorker(
            WriteTreeBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~WriteTreeWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        WriteTreeBaton *baton;
    };
    static NAN_METHOD(WriteTree);
    static NAN_METHOD(Size);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Entry);
    static NAN_METHOD(Remove);
    static NAN_METHOD(RemoveDirectory);

    struct AddBypathBaton {
      int error_code;
      const git_error* error;
      git_index * index;
      const char * path;
    };
    class AddBypathWorker : public NanAsyncWorker {
      public:
        AddBypathWorker(
            AddBypathBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~AddBypathWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        AddBypathBaton *baton;
    };
    static NAN_METHOD(AddBypath);
    static NAN_METHOD(RemoveBypath);
    static NAN_METHOD(Find);
    static NAN_METHOD(ConflictRemove);
    static NAN_METHOD(ConflictCleanup);
    static NAN_METHOD(HasConflicts);

    struct IndexToWorkdirBaton {
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      git_repository * repo;
      git_index * index;
      const git_diff_options * opts;
    };
    class IndexToWorkdirWorker : public NanAsyncWorker {
      public:
        IndexToWorkdirWorker(
            IndexToWorkdirBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~IndexToWorkdirWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        IndexToWorkdirBaton *baton;
    };
    static NAN_METHOD(IndexToWorkdir);
    git_index *raw;
};

#endif
