#ifndef NODEGIT_CLEANUP_HANDLE_H
#define NODEGIT_CLEANUP_HANDLE_H

#include <map>
#include <memory>
#include <string>

extern "C" {
  #include <git2.h>
  #include <git2/sys/filter.h>
}


namespace nodegit {
  class CleanupHandle {
  public:
    CleanupHandle();
    virtual ~CleanupHandle();
  };

  class FilterRegistryCleanupHandles : public CleanupHandle {
  public:
    ~FilterRegistryCleanupHandles() {
      for(std::map<std::string, std::shared_ptr<CleanupHandle>>::iterator iter = registeredFilters.begin(); iter != registeredFilters.end(); ++iter) {
        std::string filtername =  iter->first;
        git_filter_unregister(filtername.c_str());
      }
    }
    std::map<std::string, std::shared_ptr<CleanupHandle>> registeredFilters;
  };
}

#endif
