/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include "../vendor/libgit2/src/git2.h"

#include "repo.h"
#include "tree.h"

using namespace v8;
using namespace node;

class GitTreeEntry : EventEmitter {
  public:
    static void GitTree::Initialize (Handle<v8::Object> target);
    void SetValue(git_tree_entry* entry);

  private:
    git_tree_entry* entry;
};

