#ifndef CONVENIENTPATCH_H
#define CONVENIENTPATCH_H
// generated from class_header.h
#include <nan.h>
#include <string>

#include "async_baton.h"
#include "promise_completion.h"

extern "C" {
#include <git2.h>
}

#include "../include/typedefs.h"
#include "../include/convenient_hunk.h"

struct ConvenientLineStats {
  size_t context;
  size_t additions;
  size_t deletions;
};

struct PatchData {
  ConvenientLineStats lineStats;
  git_delta_t status;
  git_diff_file new_file;
  git_diff_file old_file;
  std::vector<HunkData *> *hunks;
  size_t numHunks;
};

PatchData *createFromRaw(git_patch *raw);
void PatchDataFree(PatchData *patch);

using namespace node;
using namespace v8;

class ConvenientPatch : public Nan::ObjectWrap {
  public:
    static Nan::Persistent<Function> constructor_template;
    static void InitializeComponent (v8::Local<v8::Object> target);

    static v8::Local<v8::Value> New(void *raw);

    ConvenientLineStats GetLineStats();
    git_delta_t GetStatus();
    git_diff_file GetOldFile();
    git_diff_file GetNewFile();
    size_t GetNumHunks();
    PatchData *GetValue();

  private:
    ConvenientPatch(PatchData *raw);
    ~ConvenientPatch();

    PatchData *patch;

    static NAN_METHOD(JSNewFunction);

    // patch methods
    static NAN_METHOD(LineStats);

    // hunk methods
    static NAN_METHOD(Size);

    struct HunksBaton {
      PatchData *patch;
      std::vector<HunkData *> *hunks;
    };
    class HunksWorker : public Nan::AsyncWorker {
      public:
        HunksWorker(
            HunksBaton *_baton,
            Nan::Callback *callback
        ) : Nan::AsyncWorker(callback)
          , baton(_baton) {};
        ~HunksWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        HunksBaton *baton;
    };

    static NAN_METHOD(Hunks);

    // delta methods
    static NAN_METHOD(OldFile);
    static NAN_METHOD(NewFile);

    // convenient status methods
    static NAN_METHOD(Status);
    static NAN_METHOD(IsUnmodified);
    static NAN_METHOD(IsAdded);
    static NAN_METHOD(IsDeleted);
    static NAN_METHOD(IsModified);
    static NAN_METHOD(IsRenamed);
    static NAN_METHOD(IsCopied);
    static NAN_METHOD(IsIgnored);
    static NAN_METHOD(IsUntracked);
    static NAN_METHOD(IsTypeChange);
    static NAN_METHOD(IsUnreadable);
    static NAN_METHOD(IsConflicted);

    // Hunk methods
};

#endif
