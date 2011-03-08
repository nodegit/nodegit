/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "reference.h"
#include "sig.h"
#include "error.h"
#include "blob.h"
#include "repo.h"
#include "oid.h"
#include "object.h"
#include "commit.h"
#include "revwalk.h"
#include "tree.h"

extern "C" void init(Handle<v8::Object> target) {
  HandleScope scope;

  Reference::Initialize(target);
  Sig::Initialize(target);
  Error::Initialize(target);
  Blob::Initialize(target);
  Oid::Initialize(target);
  GitObject::Initialize(target);
  Repo::Initialize(target);
  Commit::Initialize(target);
  RevWalk::Initialize(target);
  GitTree::Initialize(target);
}
