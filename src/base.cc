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
#include "../include/attr.h"
#include "../include/blame.h"
#include "../include/blob.h"
#include "../include/branch.h"
#include "../include/buffer.h"
#include "../include/clone.h"
#include "../include/checkout.h"
#include "../include/commit.h"
#include "../include/common.h"
#include "../include/config.h"
#include "../include/cred_helpers.h"
#include "../include/diff.h"
#include "../include/errors.h"
#include "../include/filter.h"
#include "../include/graph.h"
#include "../include/ignore.h"
#include "../include/index.h"
#include "../include/indexer.h"
#include "../include/inttypes.h"
#include "../include/merge.h"
#include "../include/message.h"
#include "../include/net.h"
#include "../include/notes.h"
#include "../include/object.h"
#include "../include/odb.h"
#include "../include/odb_backend.h"
#include "../include/oid.h"
#include "../include/pack.h"
#include "../include/patch.h"
#include "../include/pathspec.h"
#include "../include/push.h"
#include "../include/refdb.h"
#include "../include/refs.h"
#include "../include/reflog.h"
#include "../include/refspec.h"
#include "../include/remote.h"
#include "../include/repository.h"
#include "../include/reset.h"
#include "../include/revparse.h"
#include "../include/revwalk.h"
#include "../include/signature.h"
#include "../include/stash.h"
#include "../include/status.h"
#include "../include/stdint.h"
#include "../include/stdarray.h"
#include "../include/submodule.h"
#include "../include/tag.h"
#include "../include/threads.h"
#include "../include/time.h"
#include "../include/trace.h"
#include "../include/transport.h"
#include "../include/tree.h"
//#include "../include/tree_entry.h"
//#include "../include/types.h"
#include "../include/version.h"

extern "C" void init(Handle<v8::Object> target) {
  NanScope();

  Wrapper::Initialize(target);

  GitAttr::Initialize(target);
  GitBlame::Initialize(target);
  GitBlob::Initialize(target);
  GitBranch::Initialize(target);
  GitBuffer::Initialize(target);
  GitCheckout::Initialize(target);
  GitClone::Initialize(target);
  GitCommit::Initialize(target);
  GitCommon::Initialize(target);
  GitDiff::Initialize(target);
  GitErrors::Initialize(target);
  GitFilter::Initialize(target);
  GitGraph::Initialize(target);
  GitIgnore::Initialize(target);
  GitIndex::Initialize(target);
  GitIndexer::Initialize(target);
  GitInttypes::Initialize(target);
  GitMerge::Initialize(target);
  GitMessage::Initialize(target);
  GitNet::Initialize(target);
  GitNotes::Initialize(target);
  GitObject::Initialize(target);
  GitOdb::Initialize(target);
  GitOdbBackend::Initialize(target);
  GitOid::Initialize(target);
  GitPack::Initialize(target);
  GitPatch::Initialize(target);
  GitPathspec::Initialize(target);
  GitPush::Initialize(target);
  GitRefdb::Initialize(target);
  GitRefs::Initialize(target);
  GitReflog::Initialize(target);
  GitRefspec::Initialize(target);
  GitRemote::Initialize(target);
  GitRepository::Initialize(target);
  GitReset::Initialize(target);
  GitRevparse::Initialize(target);
  GitRevwalk::Initialize(target);
  GitSignature::Initialize(target);
  GitStash::Initialize(target);
  GitStatus::Initialize(target);
  GitStdint::Initialize(target);
  GitStdarray::Initialize(target);
  GitSubmodule::Initialize(target);
  GitTag::Initialize(target);
  GitThreads::Initialize(target);
  GitTime::Initialize(target);
  GitTrace::Initialize(target);
  GitTransport::Initialize(target);
  GitTree::Initialize(target);
  //GitTreeEntry::Initialize(target);
  //GitTypes::Initialize(target);
  GitVersion::Initialize(target);
}

NODE_MODULE(nodegit, init)
