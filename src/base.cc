/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "git2.h"

#include "../include/reference.h"
#include "../include/signature.h"
#include "../include/error.h"
#include "../include/blob.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/commit.h"
#include "../include/revwalk.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"
#include "../include/diff_list.h"
#include "../include/threads.h"

extern "C" void init(Handle<v8::Object> target) {
  HandleScope scope;

  GitError::Initialize(target);

  GitReference::Initialize(target);
  GitSignature::Initialize(target);
  GitBlob::Initialize(target);
  GitOid::Initialize(target);
  GitRepo::Initialize(target);
  GitCommit::Initialize(target);
  GitRevWalk::Initialize(target);

  GitTree::Initialize(target);
  GitTreeEntry::Initialize(target);

  GitDiffList::Initialize(target);

  GitThreads::Initialize(target);

}

NODE_MODULE(nodegit, init)
