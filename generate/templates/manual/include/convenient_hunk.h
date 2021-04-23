#ifndef CONVENIENTHUNK_H
#define CONVENIENTHUNK_H
// generated from class_header.h
#include <nan.h>
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

using namespace node;
using namespace v8;

class ConvenientHunk : public Nan::ObjectWrap {
  public:
    static void InitializeComponent (v8::Local<v8::Object> target, nodegit::Context *nodegitContext);

    static v8::Local<v8::Value> New(void *raw);

    HunkData *GetValue();
    char *GetHeader();
    size_t GetSize();

    void Reference();
    void Unreference();

  private:
    ConvenientHunk(HunkData *hunk);
    ConvenientHunk(const ConvenientHunk &) = delete;
    ConvenientHunk(ConvenientHunk &&) = delete;
    ConvenientHunk &operator=(const ConvenientHunk &) = delete;
    ConvenientHunk &operator=(ConvenientHunk &&) = delete;
    ~ConvenientHunk();

    HunkData *hunk;

    static NAN_METHOD(JSNewFunction);
    static NAN_METHOD(Size);

    static NAN_METHOD(OldStart);
    static NAN_METHOD(OldLines);
    static NAN_METHOD(NewStart);
    static NAN_METHOD(NewLines);
    static NAN_METHOD(HeaderLen);
    static NAN_METHOD(Header);

    struct LinesBaton {
      HunkData *hunk;
      std::vector<git_diff_line *> *lines;
    };
    class LinesWorker : public nodegit::AsyncWorker {
      public:
        LinesWorker(
            LinesBaton *_baton,
            Nan::Callback *callback
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
    static NAN_METHOD(Lines);
};

#endif
