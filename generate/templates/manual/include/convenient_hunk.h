#ifndef CONVENIENTHUNK_H
#define CONVENIENTHUNK_H
// generated from class_header.h
#include <nan.h>
#include <string>

#include "async_baton.h"
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
    static Nan::Persistent<Function> constructor_template;
    static void InitializeComponent (v8::Local<v8::Object> target);

    static v8::Local<v8::Value> New(void *raw);

    HunkData *GetValue();
    char *GetHeader();
    size_t GetSize();

  private:
    ConvenientHunk(HunkData *hunk);
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
    class LinesWorker : public Nan::AsyncWorker {
      public:
        LinesWorker(
            LinesBaton *_baton,
            Nan::Callback *callback
        ) : Nan::AsyncWorker(callback)
          , baton(_baton) {};
        ~LinesWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        LinesBaton *baton;
    };
    static NAN_METHOD(Lines);
};

#endif
