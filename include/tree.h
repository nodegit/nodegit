/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITTREE_H
#define GITTREE_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitTree : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_tree *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitTree(git_tree *raw);
    ~GitTree();

    static NAN_METHOD(New);

    static NAN_METHOD(Oid);
    static NAN_METHOD(Size);
    static NAN_METHOD(EntryByName);
    static NAN_METHOD(EntryByIndex);
    static NAN_METHOD(EntryByOid);

    struct GetEntryBaton {
      int error_code;
      const git_error* error;
      git_tree_entry * out;
      git_tree * root;
      const char * path;
    };
    class GetEntryWorker : public NanAsyncWorker {
      public:
        GetEntryWorker(
            GetEntryBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetEntryWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetEntryBaton *baton;
    };
    static NAN_METHOD(GetEntry);
    static NAN_METHOD(Builder);

    struct DiffTreeBaton {
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      git_repository * repo;
      git_tree * old_tree;
      git_tree * new_tree;
      const git_diff_options * opts;
    };
    class DiffTreeWorker : public NanAsyncWorker {
      public:
        DiffTreeWorker(
            DiffTreeBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~DiffTreeWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        DiffTreeBaton *baton;
    };
    static NAN_METHOD(DiffTree);

    struct DiffIndexBaton {
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      git_repository * repo;
      git_tree * old_tree;
      git_index * index;
      const git_diff_options * opts;
    };
    class DiffIndexWorker : public NanAsyncWorker {
      public:
        DiffIndexWorker(
            DiffIndexBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~DiffIndexWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        DiffIndexBaton *baton;
    };
    static NAN_METHOD(DiffIndex);

    struct DiffWorkDirBaton {
      int error_code;
      const git_error* error;
      git_diff_list * diff;
      git_repository * repo;
      git_tree * old_tree;
      const git_diff_options * opts;
    };
    class DiffWorkDirWorker : public NanAsyncWorker {
      public:
        DiffWorkDirWorker(
            DiffWorkDirBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~DiffWorkDirWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        DiffWorkDirBaton *baton;
    };
    static NAN_METHOD(DiffWorkDir);
    git_tree *raw;
};

#endif
