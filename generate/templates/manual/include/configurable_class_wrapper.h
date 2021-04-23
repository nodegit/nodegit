#ifndef CALLER_CONFIGURABLE_CLASS_WRAPPER_H
#define CALLER_CONFIGURABLE_CLASS_WRAPPER_H

#include <memory>
#include <nan.h>
#include <string>

#include "cleanup_handle.h"

namespace nodegit {
  class Context;

  template<typename Traits>
  class ConfigurableClassWrapper : public CleanupHandle {
  public:
    typedef typename Traits::cType cType;
    typedef typename Traits::configurableCppClass configurableCppClass;

    struct v8ConversionResult {
      v8ConversionResult(std::string _error)
        : error(std::move(_error)), result(nullptr)
      {}

      v8ConversionResult(std::shared_ptr<configurableCppClass> _result)
        : result(std::move(_result))
      {}

      std::string error;
      std::shared_ptr<configurableCppClass> result;
    };

    // We copy the entity
    ConfigurableClassWrapper(nodegit::Context *_nodeGitContext)
      : nodegitContext(_nodeGitContext), raw(nullptr) {}

    ConfigurableClassWrapper(const ConfigurableClassWrapper &) = delete;
    ConfigurableClassWrapper(ConfigurableClassWrapper &&) = delete;
    ConfigurableClassWrapper &operator=(const ConfigurableClassWrapper &) = delete;
    ConfigurableClassWrapper &operator=(ConfigurableClassWrapper &&) = delete;

    virtual ~ConfigurableClassWrapper() {
      if (raw != nullptr) {
        delete raw;
        raw = nullptr;
      }
    }

    const Context *nodegitContext = nullptr;
    cType *GetValue() {
      return raw;
    }

  protected:
    cType *raw;
    std::vector<std::shared_ptr<CleanupHandle>> childCleanupVector;
  };
}

#endif
