#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#include <unordered_map>

#include "lock_master.h"

// There are certain instances in libgit2 which can be retrieved from multiple sources
// We need to make sure that we're counting how many times we've seen that pointer
// so that when we are performing free behavior, we don't free it until it is no longer
// referenced. The main example of this behavior is the repository instance, where
// after git_repository_open open (first instance) we can git_commit_lookup, followed by
// git_commit_owner (second instance).
//
// I was hoping that we could construct a Persistent handle, but that would interfere with
// GC. We want it to attmept to GC, and if this handle exists, the final repo will not
// free itself :(.
//
// Make sure to utilize LockMaster when incrementing or decrementing a reference count.
class ReferenceCounter {
public:
  static void incrementCountForPointer(void *ptr);

  static unsigned long decrementCountForPointer(void *ptr);

private:
  static std::unordered_map<void *, unsigned long> referenceCountByPointer;
};

#endif
