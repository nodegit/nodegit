#include <napi.h>
#include <string.h>

extern "C" {
  #include <git2.h>
}

#include "../include/context.h"
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
  while(!patch->hunks->empty()) {
    HunkData *hunk = patch->hunks->back();
    patch->hunks->pop_back();
    while (!hunk->lines->empty()) {
      git_diff_line *line = hunk->lines->back();
      hunk->lines->pop_back();
      free((void *)line->content);
      free((void *)line);
    }
    delete hunk;
  }
  delete patch;
}

PatchData *createFromRaw(git_patch *raw) {
  PatchData *patch = new PatchData();
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
  patch->hunks = new std::vector<HunkData *>;
  patch->hunks->reserve(patch->numHunks);

  for (unsigned int i = 0; i < patch->numHunks; ++i) {
    HunkData *hunkData = new HunkData();
    const git_diff_hunk *hunk = NULL;
    int result = git_patch_get_hunk(&hunk, &hunkData->numLines, raw, i);
    if (result != 0) {
      continue;
    }

    hunkData->hunk.old_start = hunk->old_start;
    hunkData->hunk.old_lines = hunk->old_lines;
    hunkData->hunk.new_start = hunk->new_start;
    hunkData->hunk.new_lines = hunk->new_lines;
    hunkData->hunk.header_len = hunk->header_len;
    memcpy(&hunkData->hunk.header, &hunk->header, 128);

    hunkData->lines = new std::vector<git_diff_line *>;
    hunkData->lines->reserve(hunkData->numLines);

    static const int noNewlineStringLength = 29;
    bool EOFFlag = false;
    for (unsigned int j = 0; j < hunkData->numLines; ++j) {
      git_diff_line *storeLine = (git_diff_line *)malloc(sizeof(git_diff_line));
      const git_diff_line *line = NULL;
      int result = git_patch_get_line_in_hunk(&line, raw, i, j);
      if (result != 0) {
        continue;
      }

      if (j == 0) {
        int calculatedContentLength = line->content_len;
        if (
          calculatedContentLength > noNewlineStringLength &&
          !strncmp(
              &line->content[calculatedContentLength - noNewlineStringLength],
              "\n\\ No newline at end of file\n", (std::min)(calculatedContentLength, noNewlineStringLength)
          )) {
          EOFFlag = true;
        }
      }

      storeLine->origin = line->origin;
      storeLine->old_lineno = line->old_lineno;
      storeLine->new_lineno = line->new_lineno;
      storeLine->num_lines = line->num_lines;
      storeLine->content_len = line->content_len;
      storeLine->content_offset = line->content_offset;
      char * transferContent;
      if (EOFFlag) {
        transferContent = (char *)malloc(storeLine->content_len + noNewlineStringLength + 1);
        memcpy(transferContent, line->content, storeLine->content_len);
        memcpy(transferContent + storeLine->content_len, "\n\\ No newline at end of file\n", noNewlineStringLength);
        transferContent[storeLine->content_len + noNewlineStringLength] = '\0';
      } else {
        transferContent = (char *)malloc(storeLine->content_len + 1);
        memcpy(transferContent, line->content, storeLine->content_len);
        transferContent[storeLine->content_len] = '\0';
      }
      storeLine->content = strdup(transferContent);
      free((void *)transferContent);
      hunkData->lines->push_back(storeLine);
    }
    patch->hunks->push_back(hunkData);
  }

  return patch;
}

ConvenientPatch::ConvenientPatch(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<ConvenientPatch>(info) {
  this->patch = static_cast<PatchData *>(info[0].As<Napi::External<void>>().Data());
}

ConvenientPatch::~ConvenientPatch() {
  PatchDataFree(this->patch);
}

void ConvenientPatch::InitializeComponent(Napi::Object target, nodegit::Context *nodegitContext) {
  Napi::Env env = target.Env();
  Napi::HandleScope scope(env);

  Napi::Function tpl = Napi::Function::New(env, JSNewFunction, "ConvenientPatch");

  Napi::Object proto = tpl.Get("prototype").As<Napi::Object>();

  proto.Set("hunks", Napi::Function::New(env, Hunks, "hunks"));
  proto.Set("lineStats", Napi::Function::New(env, LineStats, "lineStats"));
  proto.Set("size", Napi::Function::New(env, Size, "size"));
  proto.Set("oldFile", Napi::Function::New(env, OldFile, "oldFile"));
  proto.Set("newFile", Napi::Function::New(env, NewFile, "newFile"));
  proto.Set("status", Napi::Function::New(env, Status, "status"));
  proto.Set("isUnmodified", Napi::Function::New(env, IsUnmodified, "isUnmodified"));
  proto.Set("isAdded", Napi::Function::New(env, IsAdded, "isAdded"));
  proto.Set("isDeleted", Napi::Function::New(env, IsDeleted, "isDeleted"));
  proto.Set("isModified", Napi::Function::New(env, IsModified, "isModified"));
  proto.Set("isRenamed", Napi::Function::New(env, IsRenamed, "isRenamed"));
  proto.Set("isCopied", Napi::Function::New(env, IsCopied, "isCopied"));
  proto.Set("isIgnored", Napi::Function::New(env, IsIgnored, "isIgnored"));
  proto.Set("isUntracked", Napi::Function::New(env, IsUntracked, "isUntracked"));
  proto.Set("isTypeChange", Napi::Function::New(env, IsTypeChange, "isTypeChange"));
  proto.Set("isUnreadable", Napi::Function::New(env, IsUnreadable, "isUnreadable"));
  proto.Set("isConflicted", Napi::Function::New(env, IsConflicted, "isConflicted"));

  nodegitContext->SaveToPersistent("ConvenientPatch::Template", tpl);
  target.Set("ConvenientPatch", tpl);
}

Napi::Value ConvenientPatch::JSNewFunction(const Napi::CallbackInfo& info) {
  if (info.Length() == 0 || !info[0].IsExternal()) {
    Napi::Error::New(info.Env(), "A new ConvenientPatch cannot be instantiated.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  new ConvenientPatch(info);

  return info.This().As<Napi::Object>();
}

Napi::Value ConvenientPatch::New(void *raw) {
  Napi::Env env = nodegit::Context::GetCurrentContext()->Env();
  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  Napi::Function constructor_template = nodegitContext->GetFromPersistent("ConvenientPatch::Template").As<Napi::Function>();
  return constructor_template.New({ Napi::External<void>::New(env, (void *)raw) });
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

Napi::Value ConvenientPatch::Hunks(const Napi::CallbackInfo& info) {
  if (info.Length() == 0 || !info[0].IsFunction()) {
    Napi::Error::New(info.Env(), "Callback is required and must be a Function.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  HunksBaton *baton = new HunksBaton();

  baton->patch = Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetValue();
  baton->hunks = new std::vector<HunkData *>;
  baton->hunks->reserve(baton->patch->numHunks);

  Napi::FunctionReference *callback = new Napi::FunctionReference(Napi::Persistent(info[0].As<Napi::Function>()));
  HunksWorker *worker = new HunksWorker(baton, callback);

  worker->Reference<ConvenientPatch>("patch", info.This().As<Napi::Object>());

  nodegit::Context *nodegitContext = nodegit::Context::GetCurrentContext();
  nodegitContext->QueueWorker(worker);
  return info.Env().Undefined();
}

nodegit::LockMaster ConvenientPatch::HunksWorker::AcquireLocks() {
  return nodegit::LockMaster(true);
}

void ConvenientPatch::HunksWorker::Execute() {
  for (unsigned int i = 0; i < baton->patch->numHunks; ++i) {
    HunkData *hunkData = new HunkData();
    hunkData->numLines = baton->patch->hunks->at(i)->numLines;
    hunkData->hunk.old_start = baton->patch->hunks->at(i)->hunk.old_start;
    hunkData->hunk.old_lines = baton->patch->hunks->at(i)->hunk.old_lines;
    hunkData->hunk.new_start = baton->patch->hunks->at(i)->hunk.new_start;
    hunkData->hunk.new_lines = baton->patch->hunks->at(i)->hunk.new_lines;
    hunkData->hunk.header_len = baton->patch->hunks->at(i)->hunk.header_len;
    memcpy(&hunkData->hunk.header, &baton->patch->hunks->at(i)->hunk.header, 128);

    hunkData->lines = new std::vector<git_diff_line *>;
    hunkData->lines->reserve(hunkData->numLines);

    for (unsigned int j = 0; j < hunkData->numLines; ++j) {
      git_diff_line *storeLine = (git_diff_line *)malloc(sizeof(git_diff_line));
      storeLine->origin = baton->patch->hunks->at(i)->lines->at(j)->origin;
      storeLine->old_lineno = baton->patch->hunks->at(i)->lines->at(j)->old_lineno;
      storeLine->new_lineno = baton->patch->hunks->at(i)->lines->at(j)->new_lineno;
      storeLine->num_lines = baton->patch->hunks->at(i)->lines->at(j)->num_lines;
      storeLine->content_len = baton->patch->hunks->at(i)->lines->at(j)->content_len;
      storeLine->content_offset = baton->patch->hunks->at(i)->lines->at(j)->content_offset;
      storeLine->content = strdup(baton->patch->hunks->at(i)->lines->at(j)->content);
      hunkData->lines->push_back(storeLine);
    }
    baton->hunks->push_back(hunkData);
  }
}

void ConvenientPatch::HunksWorker::HandleErrorCallback() {
  while (!baton->hunks->empty()) {
    HunkData *hunk = baton->hunks->back();
    baton->hunks->pop_back();

    while (!hunk->lines->empty()) {
      free(hunk->lines->back());
      hunk->lines->pop_back();
    }
  }

  delete baton->hunks;
}

void ConvenientPatch::HunksWorker::HandleOKCallback() {
  Napi::Env env = GetAsyncResource()->Env();
  unsigned int size = baton->hunks->size();
  Napi::Array result = Napi::Array::New(env, size);

  for(unsigned int i = 0; i < size; ++i) {
    result.Set(i, ConvenientHunk::New(baton->hunks->at(i)));
  }

  delete baton->hunks;

  Napi::Value argv[2] = {
    env.Null(),
    result
  };
  CallCallback(argv, 2);

  delete baton;
}

Napi::Value ConvenientPatch::LineStats(const Napi::CallbackInfo& info) {
  Napi::Object toReturn = Napi::Object::New(info.Env());
  ConvenientLineStats stats = Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetLineStats();

  toReturn.Set("total_context", Napi::Number::New(info.Env(), stats.context));
  toReturn.Set("total_additions", Napi::Number::New(info.Env(), stats.additions));
  toReturn.Set("total_deletions", Napi::Number::New(info.Env(), stats.deletions));

  return toReturn;
}

Napi::Value ConvenientPatch::Size(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetNumHunks());
}

Napi::Value ConvenientPatch::OldFile(const Napi::CallbackInfo& info) {
  git_diff_file *old_file = (git_diff_file *)malloc(sizeof(git_diff_file));
  *old_file = Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetOldFile();
  return GitDiffFile::New(old_file, true);
}

Napi::Value ConvenientPatch::NewFile(const Napi::CallbackInfo& info) {
  git_diff_file *new_file = (git_diff_file *)malloc(sizeof(git_diff_file));
  *new_file = Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetNewFile();
  if (new_file != NULL) {
    return GitDiffFile::New(new_file, true);
  } else {
    return info.Env().Null();
  }
}

Napi::Value ConvenientPatch::Status(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus());
}

Napi::Value ConvenientPatch::IsUnmodified(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_UNMODIFIED);
}

Napi::Value ConvenientPatch::IsAdded(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_ADDED);
}

Napi::Value ConvenientPatch::IsDeleted(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_DELETED);
}

Napi::Value ConvenientPatch::IsModified(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_MODIFIED);
}

Napi::Value ConvenientPatch::IsRenamed(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_RENAMED);
}

Napi::Value ConvenientPatch::IsCopied(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_COPIED);
}

Napi::Value ConvenientPatch::IsIgnored(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_IGNORED);
}

Napi::Value ConvenientPatch::IsUntracked(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_UNTRACKED);
}

Napi::Value ConvenientPatch::IsTypeChange(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_TYPECHANGE);
}

Napi::Value ConvenientPatch::IsUnreadable(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_UNREADABLE);
}

Napi::Value ConvenientPatch::IsConflicted(const Napi::CallbackInfo& info) {
  return Napi::Boolean::New(info.Env(), Napi::ObjectWrap<ConvenientPatch>::Unwrap(info.This().As<Napi::Object>())->GetStatus() == GIT_DELTA_CONFLICTED);
}

void ConvenientPatch::Reference() {
  Ref();
}

void ConvenientPatch::Unreference() {
  Unref();
}
