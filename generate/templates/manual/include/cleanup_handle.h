#ifndef NODEGIT_CLEANUP_HANDLE_H
#define NODEGIT_CLEANUP_HANDLE_H

#include <map>
#include <memory>
#include <string>

namespace nodegit {
  class CleanupHandle {
  public:
    CleanupHandle();
    virtual ~CleanupHandle();
  };

  class FilterRegistryCleanupHandles : public CleanupHandle {
  public:
    std::map<std::string, std::shared_ptr<CleanupHandle>> registeredFilters;
  };
}

#endif
