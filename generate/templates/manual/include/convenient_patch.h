#ifndef CONVENIENTPATCH_H
#define CONVENIENTPATCH_H
// generated from class_header.h
#include <napi.h>
#include <string>

#include "async_baton.h"
#include "async_worker.h"
#include "lock_master.h"
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

class ConvenientPatch : public Napi::ObjectWrap<ConvenientPatch> {
  public:
    ConvenientPatch(const Napi::CallbackInfo& info);
    ConvenientPatch(const ConvenientPatch &) = delete;
    ConvenientPatch(ConvenientPatch &&) = delete;
    ConvenientPatch &operator=(const ConvenientPatch &) = delete;
    ConvenientPatch &operator=(ConvenientPatch &&) = delete;

    static void InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext);

    static Napi::Value New(void *raw);

    ConvenientLineStats GetLineStats();
    git_delta_t GetStatus();
    git_diff_file GetOldFile();
    git_diff_file GetNewFile();
    size_t GetNumHunks();
    PatchData *GetValue();

    void Reference();
    void Unreference();

    ~ConvenientPatch();

  private:
    PatchData *patch;

    static Napi::Value JSNewFunction(const Napi::CallbackInfo& info);

    // patch methods
    static Napi::Value LineStats(const Napi::CallbackInfo& info);

    // hunk methods
    static Napi::Value Size(const Napi::CallbackInfo& info);

    struct HunksBaton {
      PatchData *patch;
      std::vector<HunkData *> *hunks;
    };
    class HunksWorker : public nodegit::AsyncWorker {
      public:
        HunksWorker(
            HunksBaton *_baton,
            Napi::FunctionReference *callback
        ) : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:ConvenientPatch:Hunks")
          , baton(_baton) {};
        HunksWorker(const HunksWorker &) = delete;
        HunksWorker(HunksWorker &&) = delete;
        HunksWorker &operator=(const HunksWorker &) = delete;
        HunksWorker &operator=(HunksWorker &&) = delete;
        ~HunksWorker(){};
        void Execute();
        void HandleErrorCallback();
        void HandleOKCallback();
        nodegit::LockMaster AcquireLocks();

      private:
        HunksBaton *baton;
    };

    static Napi::Value Hunks(const Napi::CallbackInfo& info);

    // delta methods
    static Napi::Value OldFile(const Napi::CallbackInfo& info);
    static Napi::Value NewFile(const Napi::CallbackInfo& info);

    // convenient status methods
    static Napi::Value Status(const Napi::CallbackInfo& info);
    static Napi::Value IsUnmodified(const Napi::CallbackInfo& info);
    static Napi::Value IsAdded(const Napi::CallbackInfo& info);
    static Napi::Value IsDeleted(const Napi::CallbackInfo& info);
    static Napi::Value IsModified(const Napi::CallbackInfo& info);
    static Napi::Value IsRenamed(const Napi::CallbackInfo& info);
    static Napi::Value IsCopied(const Napi::CallbackInfo& info);
    static Napi::Value IsIgnored(const Napi::CallbackInfo& info);
    static Napi::Value IsUntracked(const Napi::CallbackInfo& info);
    static Napi::Value IsTypeChange(const Napi::CallbackInfo& info);
    static Napi::Value IsUnreadable(const Napi::CallbackInfo& info);
    static Napi::Value IsConflicted(const Napi::CallbackInfo& info);
};

#endif
