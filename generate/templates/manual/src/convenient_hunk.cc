#include <napi.h>
#include <string.h>

extern "C" {
  #include <git2.h>
}

#include "../include/context.h"
#include "../include/functions/copy.h"
#include "../include/convenient_hunk.h"
#include "../include/diff_line.h"

using namespace std;
using namespace v8;
using namespace node;

void HunkDataFree(HunkData *hunk) {
  while (!hunk->lines->empty()) {
    git_diff_line *line = hunk->lines->back();
    hunk->lines->pop_back();
    free((void *)line->content);
    free((void *)line);
  }
  delete hunk->lines;
  delete hunk;
}

ConvenientHunk::ConvenientHunk(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<ConvenientHunk>(info) {
  HunkData *raw = static_cast<HunkData *>(info[0].As<Napi::External<void>>().Data());
  this->hunk = raw;
}

ConvenientHunk::~ConvenientHunk() {
  HunkDataFree(this->hunk);
}

void ConvenientHunk::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
  Napi::Env env = target.Env();
  Napi::HandleScope scope(env);

  Napi::Function tpl = Napi::Function::New(env, JSNewFunction, "ConvenientHunk");

  Napi::Object proto = tpl.Get("prototype").As<Napi::Object>();

  proto.Set("size", Napi::Function::New(env, Size, "size"));
  proto.Set("lines", Napi::Function::New(env, Lines, "lines"));
  proto.Set("oldStart", Napi::Function::New(env, OldStart, "oldStart"));
  proto.Set("oldLines", Napi::Function::New(env, OldLines, "oldLines"));
  proto.Set("newStart", Napi::Function::New(env, NewStart, "newStart"));
  proto.Set("newLines", Napi::Function::New(env, NewLines, "newLines"));
  proto.Set("headerLen", Napi::Function::New(env, HeaderLen, "headerLen"));
  proto.Set("header", Napi::Function::New(env, Header, "header"));

  nodegitContext->SaveToPersistent("ConvenientHunk::Template", tpl);
  target.Set("ConvenientHunk", tpl);
}

Napi::Value ConvenientHunk::JSNewFunction(const Napi::CallbackInfo& info) {
  if (info.Length() == 0 || !info[0].IsExternal()) {
    Napi::Error::New(info.Env(), "A new ConvenientHunk cannot be instantiated.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  new ConvenientHunk(info);

  return info.This().As<Napi::Object>();
}

Napi::Value ConvenientHunk::New(void *raw) {
  Napi::Env env = nodegit::Context::GetCurrentContext()->Env();
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  Napi::Function constructor_template = nodegitContext->GetFromPersistent("ConvenientHunk::Template").As<Napi::Function>();
  return constructor_template.New({ Napi::External<void>::New(env, (void *)raw) });
}

HunkData *ConvenientHunk::GetValue() {
  return this->hunk;
}

size_t ConvenientHunk::GetSize() {
  return this->hunk->numLines;
}

Napi::Value ConvenientHunk::Size(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  return Napi::Number::New(info.Env(), self->GetSize());
}

Napi::Value ConvenientHunk::Lines(const Napi::CallbackInfo& info) {
  if (info.Length() == 0 || !info[0].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  LinesBaton *baton = new LinesBaton();

  baton->hunk = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>())->GetValue();
  baton->lines = new std::vector<git_diff_line *>;
  baton->lines->reserve(baton->hunk->numLines);

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[0].As<Napi::Function>()));
  LinesWorker *worker = new LinesWorker(baton, callback);

  worker->Reference<ConvenientHunk>("hunk", info.This().As<Napi::Object>());

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
}

nodegit::LockMaster ConvenientHunk::LinesWorker::AcquireLocks() {
  return nodegit::LockMaster(true);
}

void ConvenientHunk::LinesWorker::Execute() {
  for (unsigned int i = 0; i < baton->hunk->numLines; ++i) {
    git_diff_line *storeLine = (git_diff_line *)malloc(sizeof(git_diff_line));
    storeLine->origin = baton->hunk->lines->at(i)->origin;
    storeLine->old_lineno = baton->hunk->lines->at(i)->old_lineno;
    storeLine->new_lineno = baton->hunk->lines->at(i)->new_lineno;
    storeLine->num_lines = baton->hunk->lines->at(i)->num_lines;
    storeLine->content_len = baton->hunk->lines->at(i)->content_len;
    storeLine->content_offset = baton->hunk->lines->at(i)->content_offset;
    storeLine->content = strdup(baton->hunk->lines->at(i)->content);
    baton->lines->push_back(storeLine);
  }
}

void ConvenientHunk::LinesWorker::HandleErrorCallback() {
  while (!baton->lines->empty()) {
    free(baton->lines->back());
    baton->lines->pop_back();
  }

  delete baton->lines;
}

void ConvenientHunk::LinesWorker::HandleOKCallback() {
  Napi::Env env = GetAsyncResource()->Env();
  unsigned int size = baton->lines->size();
  Napi::Array result = Napi::Array::New(env, size);

  for(unsigned int i = 0; i < size; ++i) {
    result.Set(i, GitDiffLine::New(baton->lines->at(i), true));
  }

  delete baton->lines;

  Napi::Value argv[2] = {
    env.Null(),
    result
  };
  CallCallback(argv, 2);

  delete baton;
}

Napi::Value ConvenientHunk::OldStart(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  int old_start = self->GetValue()->hunk.old_start;
  return Napi::Number::New(info.Env(), old_start);
}

Napi::Value ConvenientHunk::OldLines(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  int old_lines = self->GetValue()->hunk.old_lines;
  return Napi::Number::New(info.Env(), old_lines);
}

Napi::Value ConvenientHunk::NewStart(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  int new_start = self->GetValue()->hunk.new_start;
  return Napi::Number::New(info.Env(), new_start);
}

Napi::Value ConvenientHunk::NewLines(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  int new_lines = self->GetValue()->hunk.new_lines;
  return Napi::Number::New(info.Env(), new_lines);
}

Napi::Value ConvenientHunk::HeaderLen(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  size_t header_len = self->GetValue()->hunk.header_len;
  return Napi::Number::New(info.Env(), header_len);
}

Napi::Value ConvenientHunk::Header(const Napi::CallbackInfo& info) {
  ConvenientHunk *self = Napi::ObjectWrap<ConvenientHunk>::Unwrap(info.This().As<Napi::Object>());
  char *header = self->GetValue()->hunk.header;
  if (header) {
    return Napi::String::New(info.Env(), header);
  } else {
    return info.Env().Null();
  }
}

void ConvenientHunk::Reference() {
  Ref();
}

void ConvenientHunk::Unreference() {
  Unref();
}
