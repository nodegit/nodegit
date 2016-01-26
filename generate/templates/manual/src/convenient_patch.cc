#include <nan.h>
#include <string.h>

extern "C" {
  #include <git2.h>
}

#include "../include/convenient_hunk.h"
#include "../include/convenient_patch.h"
#include "../include/functions/copy.h"
#include "../include/diff_file.h"

using namespace std;
using namespace v8;
using namespace node;

void PatchDataFree(PatchData *patch) {
  free((void *)patch->old_file.path);
  free((void *)patch->new_file.path);
  for (unsigned int i = 0; i < patch->numHunks; ++i) {
    for (unsigned int j = 0; j < patch->hunks[i].numLines; ++j) {
      free((void *)patch->hunks[i].lines[j].content);
    }
    free((void *)patch->hunks[i].lines);
  }
  delete[] patch->hunks;
  delete patch;
}

PatchData *createFromRaw(git_patch *raw) {
  PatchData *patch = new PatchData;
  const git_diff_delta *delta = git_patch_get_delta(raw);

  patch->status = delta->status;

  patch->old_file = delta->old_file;
  patch->old_file.path = strdup(delta->old_file.path);

  patch->new_file = delta->new_file;
  patch->new_file.path = strdup(delta->new_file.path);

  git_patch_line_stats(
    &patch->lineStats.context,
    &patch->lineStats.additions,
    &patch->lineStats.deletions,
    raw
  );

  patch->numHunks = git_patch_num_hunks(raw);
  patch->hunks = new HunkData[patch->numHunks];

  for (unsigned int i = 0; i < patch->numHunks; ++i) {
    const git_diff_hunk *hunk;
    git_patch_get_hunk(&hunk, &patch->hunks[i].numLines, raw, i);
    patch->hunks[i].hunk = *hunk;
    patch->hunks[i].lines = (git_diff_line *)malloc(sizeof(git_diff_line) * patch->hunks[i].numLines);

    for (unsigned int j = 0; j < patch->hunks[i].numLines; ++j) {
      const git_diff_line *line;
      git_patch_get_line_in_hunk(&line, raw, i, j);
      patch->hunks[i].lines[j] = *line;
      patch->hunks[i].lines[j].content = strdup(line->content);
    }
  }

  return patch;
}

ConvenientPatch::ConvenientPatch(PatchData *raw) {
  this->patch = raw;
}

ConvenientPatch::~ConvenientPatch() {
  PatchDataFree(this->patch);
}

void ConvenientPatch::InitializeComponent(Local<v8::Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(JSNewFunction);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(Nan::New("ConvenientPatch").ToLocalChecked());

  Nan::SetPrototypeMethod(tpl, "hunks", Hunks);
  Nan::SetPrototypeMethod(tpl, "lineStats", LineStats);
  Nan::SetPrototypeMethod(tpl, "size", Size);

  Nan::SetPrototypeMethod(tpl, "oldFile", OldFile);
  Nan::SetPrototypeMethod(tpl, "newFile", NewFile);
  Nan::SetPrototypeMethod(tpl, "status", Status);
  Nan::SetPrototypeMethod(tpl, "isUnmodified", IsUnmodified);
  Nan::SetPrototypeMethod(tpl, "isAdded", IsAdded);
  Nan::SetPrototypeMethod(tpl, "isDeleted", IsDeleted);
  Nan::SetPrototypeMethod(tpl, "isModified", IsModified);
  Nan::SetPrototypeMethod(tpl, "isRenamed", IsRenamed);
  Nan::SetPrototypeMethod(tpl, "isCopied", IsCopied);
  Nan::SetPrototypeMethod(tpl, "isIgnored", IsIgnored);
  Nan::SetPrototypeMethod(tpl, "isUntracked", IsUntracked);
  Nan::SetPrototypeMethod(tpl, "isTypeChange", IsTypeChange);
  Nan::SetPrototypeMethod(tpl, "isUnreadable", IsUnreadable);
  Nan::SetPrototypeMethod(tpl, "isConflicted", IsConflicted);

  Local<Function> _constructor_template = Nan::GetFunction(tpl).ToLocalChecked();
  constructor_template.Reset(_constructor_template);
  Nan::Set(target, Nan::New("ConvenientPatch").ToLocalChecked(), _constructor_template);
}

NAN_METHOD(ConvenientPatch::JSNewFunction) {

  if (info.Length() == 0 || !info[0]->IsExternal()) {
       return Nan::ThrowError("A new ConvenientPatch cannot be instantiated.");
   }

  ConvenientPatch* object = new ConvenientPatch(static_cast<PatchData *>(Local<External>::Cast(info[0])->Value()));
  object->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

Local<v8::Value> ConvenientPatch::New(void *raw) {
  Nan::EscapableHandleScope scope;
  Local<v8::Value> argv[1] = { Nan::New<External>((void *)raw) };
  return scope.Escape(Nan::NewInstance(Nan::New(ConvenientPatch::constructor_template), 1, argv).ToLocalChecked());
}

ConvenientLineStats ConvenientPatch::GetLineStats() {
  return this->patch->lineStats;
}

git_delta_t ConvenientPatch::GetStatus() {
  return this->patch->status;
}

git_diff_file ConvenientPatch::GetOldFile() {
  return this->patch->old_file;
}

git_diff_file ConvenientPatch::GetNewFile() {
  return this->patch->new_file;
}

size_t ConvenientPatch::GetNumHunks() {
  return this->patch->numHunks;
}

PatchData *ConvenientPatch::GetValue() {
  return this->patch;
}

NAN_METHOD(ConvenientPatch::Hunks) {
  if (info.Length() == 0 || !info[0]->IsFunction()) {
    return Nan::ThrowError("Callback is required and must be a Function.");
  }

  HunksBaton *baton = new HunksBaton;

  baton->patch = Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetValue();

  Nan::Callback *callback = new Nan::Callback(Local<Function>::Cast(info[0]));
  HunksWorker *worker = new HunksWorker(baton, callback);

  worker->SaveToPersistent("patch", info.This());

  Nan::AsyncQueueWorker(worker);
  return;
}

void ConvenientPatch::HunksWorker::Execute() {
  // copy hunks
  baton->hunks = new HunkData *[baton->patch->numHunks];
  for (unsigned int i = 0; i < baton->patch->numHunks; ++i) {
    baton->hunks[i] = new HunkData;
    *baton->hunks[i] = baton->patch->hunks[i];
    baton->hunks[i]->lines = (git_diff_line *)malloc(baton->hunks[i]->numLines);
    memcpy(
      baton->hunks[i]->lines,
      baton->patch->hunks[i].lines,
      sizeof(git_diff_line) * baton->hunks[i]->numLines
    );
    for (unsigned int j = 0; j < baton->hunks[i]->numLines; ++j) {
      baton->hunks[i]->lines[j].content = strdup(baton->patch->hunks[i].lines[j].content);
    }
  }
}

void ConvenientPatch::HunksWorker::HandleOKCallback() {
  unsigned int size = baton->patch->numHunks;
  Local<Array> result = Nan::New<Array>(size);

  for(unsigned int i = 0; i < size; ++i) {
    Nan::Set(result, Nan::New<Number>(i), ConvenientHunk::New(baton->hunks[i]));
  }

  delete baton->hunks;

  Local<v8::Value> argv[2] = {
    Nan::Null(),
    result
  };
  callback->Call(2, argv);
}

NAN_METHOD(ConvenientPatch::LineStats) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> to;
  Local<Object> toReturn = Nan::New<Object>();
  ConvenientLineStats stats = Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetLineStats();

  to = Nan::New<Number>(stats.context);
  Nan::Set(toReturn, Nan::New("total_context").ToLocalChecked(), to);
  to = Nan::New<Number>(stats.additions);
  Nan::Set(toReturn, Nan::New("total_additions").ToLocalChecked(), to);
  to = Nan::New<Number>(stats.deletions);
  Nan::Set(toReturn, Nan::New("total_deletions").ToLocalChecked(), to);

  return info.GetReturnValue().Set(scope.Escape(toReturn));
}

NAN_METHOD(ConvenientPatch::Size) {
  Local<v8::Value> to;

  to = Nan::New<Number>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetNumHunks());

  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::OldFile) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> to;
  git_diff_file *old_file = (git_diff_file *)malloc(sizeof(git_diff_file));
  *old_file = Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetOldFile();

  to = GitDiffFile::New((void *)old_file, true);

  return info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::NewFile) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> to;
  git_diff_file *new_file = (git_diff_file *)malloc(sizeof(git_diff_file));
  *new_file = Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetNewFile();
  if (new_file != NULL) {
    to = GitDiffFile::New((void *)new_file, true);
  } else {
    to = Nan::Null();
  }

  return info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::Status) {
  Local<v8::Value> to;
  to = Nan::New<Number>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus());
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsUnmodified) {
  Nan::EscapableHandleScope scope;

  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_UNMODIFIED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsAdded) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_ADDED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsDeleted) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_DELETED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsModified) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_MODIFIED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsRenamed) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_RENAMED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsCopied) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_COPIED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsIgnored) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_IGNORED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsUntracked) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_UNTRACKED);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsTypeChange) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_TYPECHANGE);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsUnreadable) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_UNREADABLE);
  info.GetReturnValue().Set(to);
}

NAN_METHOD(ConvenientPatch::IsConflicted) {
  Local<v8::Value> to;
  to = Nan::New<Boolean>(Nan::ObjectWrap::Unwrap<ConvenientPatch>(info.This())->GetStatus() == GIT_DELTA_CONFLICTED);
  info.GetReturnValue().Set(to);
}

Nan::Persistent<Function> ConvenientPatch::constructor_template;
