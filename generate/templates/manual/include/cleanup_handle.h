#ifndef NODEGIT_CLEANUP_HANDLE_H
#define NODEGIT_CLEANUP_HANDLE_H

#include <map>
#include <memory>

namespace nodegit {
  class CleanupHandle {
  public:
    CleanupHandle();
    virtual ~CleanupHandle();
  };
}

#endif
