/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "git2.h"

#include "../include/wrapper.h"
#include "../include/reference.h"
#include "../include/signature.h"
#include "../include/time.h"
#include "../include/blob.h"
#include "../include/repo.h"
#include "../include/oid.h"
#include "../include/object.h"
#include "../include/commit.h"
#include "../include/revwalk.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"
#include "../include/diff_find_options.h"
#include "../include/diff_options.h"
#include "../include/diff_list.h"
#include "../include/diff_range.h"
#include "../include/diff_file.h"
#include "../include/patch.h"
#include "../include/delta.h"
#include "../include/threads.h"
#include "../include/index.h"
#include "../include/index_entry.h"
#include "../include/index_time.h"
#include "../include/tag.h"
#include "../include/refdb.h"
#include "../include/odb_object.h"
#include "../include/odb.h"
#include "../include/submodule.h"
#include "../include/tree_builder.h"
#include "../include/remote.h"
#include "../include/clone_options.h"

extern "C" void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::Initialize(target);

  GitReference::Initialize(target);
  GitIndex::Initialize(target);
  GitIndexEntry::Initialize(target);
  GitIndexTime::Initialize(target);
  GitTag::Initialize(target);
  GitSignature::Initialize(target);
  GitTime::Initialize(target);
  GitBlob::Initialize(target);
  GitOid::Initialize(target);
  GitObject::Initialize(target);
  GitRepo::Initialize(target);
  GitCommit::Initialize(target);
  GitRevWalk::Initialize(target);
  GitRefDb::Initialize(target);
  GitOdb::Initialize(target);
  GitOdbObject::Initialize(target);
  GitSubmodule::Initialize(target);

  GitTree::Initialize(target);
  GitTreeEntry::Initialize(target);
  GitTreeBuilder::Initialize(target);

  GitDiffRange::Initialize(target);
  GitDiffFindOptions::Initialize(target);
  GitDiffOptions::Initialize(target);
  GitDiffList::Initialize(target);
  GitPatch::Initialize(target);
  GitDiffFile::Initialize(target);
  GitDelta::Initialize(target);

  GitRemote::Initialize(target);
  GitCloneOptions::Initialize(target);

  GitThreads::Initialize(target);

}

NODE_MODULE(nodegit, init)
