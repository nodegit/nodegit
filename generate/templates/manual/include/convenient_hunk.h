#ifndef CONVENIENTHUNK_H
#define CONVENIENTHUNK_H
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

struct HunkData {
  git_diff_hunk hunk;
  std::vector<git_diff_line *> *lines;
  size_t numLines;
};

void HunkDataFree(HunkData *hunk);

class ConvenientHunk : public Napi::ObjectWrap<ConvenientHunk> {
  public:
    ConvenientHunk(const Napi::CallbackInfo& info);
    static void InitializeComponent (Napi::Object target, nodegit::Context *nodegitContext);

    static Napi::Value New(void *raw);

    HunkData *GetValue();
    char *GetHeader();
    size_t GetSize();

    void Reference();
    void Unreference();

    ~ConvenientHunk();

  private:
    ConvenientHunk(const ConvenientHunk &) = delete;
    ConvenientHunk(ConvenientHunk &&) = delete;
    ConvenientHunk &operator=(const ConvenientHunk &) = delete;
    ConvenientHunk &operator=(ConvenientHunk &&) = delete;

    HunkData *hunk;

    static Napi::Value JSNewFunction(const Napi::CallbackInfo& info);
    static Napi::Value Size(const Napi::CallbackInfo& info);

    static Napi::Value OldStart(const Napi::CallbackInfo& info);
    static Napi::Value OldLines(const Napi::CallbackInfo& info);
    static Napi::Value NewStart(const Napi::CallbackInfo& info);
    static Napi::Value NewLines(const Napi::CallbackInfo& info);
    static Napi::Value HeaderLen(const Napi::CallbackInfo& info);
    static Napi::Value Header(const Napi::CallbackInfo& info);
    static Napi::Value Lines(const Napi::CallbackInfo& info);

    struct LinesBaton {
      HunkData *hunk;
      std::vector<git_diff_line *> *lines;
    };
    class LinesWorker : public nodegit::AsyncWorker {
      public:
        LinesWorker(
            LinesBaton *_baton,
            Napi::FunctionReference *callback
        ) : nodegit::AsyncWorker(callback, "nodegit:AsyncWorker:ConvenientHunk:Lines")
          , baton(_baton) {};
        LinesWorker(const LinesWorker &) = delete;
        LinesWorker(LinesWorker &&) = delete;
        LinesWorker &operator=(const LinesWorker &) = delete;
        LinesWorker &operator=(LinesWorker &&) = delete;
        ~LinesWorker(){};
        void Execute();
        void HandleErrorCallback();
        void HandleOKCallback();
        nodegit::LockMaster AcquireLocks();

      private:
        LinesBaton *baton;
    };
};

#endif
