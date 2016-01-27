#include <nan.h>
#include <string.h>

extern "C" {
  #include <git2.h>
}

#include "../include/functions/copy.h"
#include "../include/convenient_hunk.h"
#include "../include/diff_line.h"

using namespace std;
using namespace v8;
using namespace node;

void HunkDataFree(HunkData *hunk) {
  while (!hunk->lines.empty()) {
    git_diff_line *line = hunk->lines.back();
    hunk->lines.pop_back();
    free((void *)line->content);
    free((void *)line);
  }
  delete hunk;
}

ConvenientHunk::ConvenientHunk(HunkData *raw) {
  this->hunk = raw;
}

ConvenientHunk::~ConvenientHunk() {
  HunkDataFree(this->hunk);
}

void ConvenientHunk::InitializeComponent(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("ConvenientHunk").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "header", Header);
  Nan::SetPrototypeMethod(tpl, "size", Size);
  Nan::SetPrototypeMethod(tpl, "lines", Lines);

  Local<Function> _constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
  constructor_template.Reset(_constructor_template);
  Nan::Set(target, Nan::New("ConvenientHunk").ToLocalChecked(), _constructor_template);
}

NAN_METHOD(ConvenientHunk::JSNewFunction) {

  if (info.Length() == 0 || !info[0]->IsExternal()) {
       return Nan::ThrowError("A new ConvenientHunk cannot be instantiated.");
   }

  ConvenientHunk* object = new ConvenientHunk(static_cast<HunkData *>(Local<External>::Cast(info[0])->Value()));
  object->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

Local<v8::Value> ConvenientHunk::New(void *raw) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[1] = { Nan::New<External>((void *)raw) };
  return scope.Escape(Nan::NewInstance(Nan::New(ConvenientHunk::constructor_template), 1, argv).ToLocalChecked());
}

HunkData *ConvenientHunk::GetValue() {
  return this->hunk;
}

char *ConvenientHunk::GetHeader() {
  return this->hunk->hunk.header;
}

size_t ConvenientHunk::GetSize() {
  return this->hunk->numLines;
}

NAN_METHOD(ConvenientHunk::Header) {
  Local<v8::Value> to;

  char *header = Nan::ObjectWrap::Unwrap<ConvenientHunk>(info.This())->GetHeader();
  if (header) {
    to = Nan::New<String>(header).ToLocalChecked();
  } else {
    to = Nan::Null();
  }

  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientHunk::Size) {
  Local<v8::Value> to;
  to = Nan::New<Number>(Nan::ObjectWrap::Unwrap<ConvenientHunk>(info.This())->GetSize());
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientHunk::Lines) {
  if (info.Length() == 0 || !info[0]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  LinesBaton *baton = new LinesBaton;

  baton->hunk = Nan::ObjectWrap::Unwrap<ConvenientHunk>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[0]));
  LinesWorker *worker = new LinesWorker(baton, callback);

  worker->SaveToPersistent("hunk", info.This());

  Nan::AsyncQueueWorker(worker);
  return;
}

void ConvenientHunk::LinesWorker::Execute() {
  baton->lines = new std::vector<git_diff_line *>;
  baton->lines->reserve(baton->hunk->numLines);
  for (unsigned int i = 0; i < baton->hunk->numLines; ++i) {
    git_diff_line *storeLine = (git_diff_line *)malloc(sizeof(git_diff_line));
    storeLine->origin = baton->hunk->lines[i]->origin;
    storeLine->old_lineno = baton->hunk->lines[i]->old_lineno;
    storeLine->new_lineno = baton->hunk->lines[i]->new_lineno;
    storeLine->num_lines = baton->hunk->lines[i]->num_lines;
    storeLine->content_len = baton->hunk->lines[i]->content_len;
    storeLine->content_offset = baton->hunk->lines[i]->content_offset;
    storeLine->content = strdup(baton->hunk->lines[i]->content);
    baton->lines->push_back(storeLine);
  }
}

void ConvenientHunk::LinesWorker::HandleOKCallback() {
  unsigned int size = baton->lines->size();
  Local<Array> result = Nan::New<Array>(size);

  for(unsigned int i = 0; i < size; ++i) {
    Nan::Set(result, Nan::New<Number>(i), GitDiffLine::New(baton->lines->at(i), true));
  }

  delete baton->lines;

  Local<v8::Value> argv[2] = {
    Nan::Null(),
    result
  };
  callback->Call(2, argv);
}

Nan::Persistent<Function> ConvenientHunk::constructor_template;
