/*
 * Copyright 2013, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#include <v8.h>
#include <node.h>

#include "git2.h"

#include "../include/blob.h"
#include "../include/commit.h"
#include "../include/config.h"
//#include "../include/diff.h"
#include "../include/index.h"
#include "../include/object.h"
#include "../include/odb.h"
#include "../include/oid.h"
#include "../include/refdb.h"
#include "../include/refs.h"
#include "../include/repository.h"
#include "../include/revwalk.h"
#include "../include/signature.h"
#include "../include/threads.h"
#include "../include/time.h"
#include "../include/trace.h"
#include "../include/tree.h"
#include "../include/tree_entry.h"
//#include "../include/treebuilder.h"
#include "../include/wrapper.h"

extern "C" void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::Initialize(target);

  GitBlob::Initialize(target);
  GitCommit::Initialize(target);
  GitConfig::Initialize(target);
  //GitDiff::Initialize(target);
  GitIndex::Initialize(target);
  GitObject::Initialize(target);
  GitOdb::Initialize(target);
  GitOid::Initialize(target);
  GitRefdb::Initialize(target);
  GitRefs::Initialize(target);
  GitRepository::Initialize(target);
  GitRevwalk::Initialize(target);
  GitSignature::Initialize(target);
  GitThreads::Initialize(target);
  GitTime::Initialize(target);
  GitTree::Initialize(target);
  GitTreeEntry::Initialize(target);
//  GitTrace::Initialize(target);
//  GitTreebuilder::Initialize(target);
}

NODE_MODULE(nodegit, init)
