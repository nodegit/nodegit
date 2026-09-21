#include "../include/v8_helpers.h"

namespace nodegit {
  Napi::Value safeGetField(Napi::Object containerObj, std::string field) {
    return containerObj.Get(field);
  }
}
