#include "../../include/oid.h"
#include "../../include/functions/commit.h"

/**
 * Non blocking, Converts GitCommitDetails struct into a v8::Local<v8::Object> for JavaScript
 */
v8::Local<v8::Object> createCommitDetailsObject(GitCommitDetails* rawDetails) {

  v8::Local<v8::Object> details = v8::Object::New();

  v8::Handle<v8::Object> oid = GitOid::constructor_template->NewInstance();
  GitOid *oidInstance = node::ObjectWrap::Unwrap<GitOid>(oid);
  oidInstance->SetValue(*const_cast<git_oid *>(rawDetails->oid));

  details->Set(String::NewSymbol("id"), oid);
  details->Set(String::NewSymbol("sha"), String::New(rawDetails->sha));
  details->Set(String::NewSymbol("message"), cvv8::CastToJS(rawDetails->message));
  details->Set(String::NewSymbol("time"), cvv8::CastToJS(rawDetails->time));
  details->Set(String::NewSymbol("timeOffset"), cvv8::CastToJS(rawDetails->timeOffset));

  v8::Local<v8::Object> committer = v8::Object::New();
  committer->Set(String::NewSymbol("name"), cvv8::CastToJS(rawDetails->committer->name));
  committer->Set(String::NewSymbol("email"), cvv8::CastToJS(rawDetails->committer->email));

  v8::Local<v8::Object> committerWhen = v8::Object::New();
  committerWhen->Set(String::NewSymbol("time"), cvv8::CastToJS(rawDetails->committer->when.time));
  committerWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(rawDetails->committer->when.offset));
  committer->Set(String::NewSymbol("when"), cvv8::CastToJS(committerWhen));

  details->Set(String::NewSymbol("committer"), committer);

  v8::Local<v8::Object> author = v8::Object::New();
  author->Set(String::NewSymbol("name"), cvv8::CastToJS(rawDetails->author->name));
  author->Set(String::NewSymbol("email"), cvv8::CastToJS(rawDetails->author->email));

  v8::Local<v8::Object> authorWhen = v8::Object::New();
  authorWhen->Set(String::NewSymbol("time"), cvv8::CastToJS(rawDetails->author->when.time));
  authorWhen->Set(String::NewSymbol("offset"), cvv8::CastToJS(rawDetails->author->when.offset));
  author->Set(String::NewSymbol("when"), authorWhen);

  details->Set(String::NewSymbol("author"), author);

  details->Set(String::NewSymbol("parentCount"), cvv8::CastToJS(rawDetails->parentCount));
  details->Set(String::NewSymbol("parentShas"), cvv8::CastToJS(rawDetails->parentShas));

  return details;
}
