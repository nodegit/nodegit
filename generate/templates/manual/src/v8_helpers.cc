#include "../include/v8_helpers.h"

namespace nodegit {
  v8::Local<v8::Value> safeGetField(v8::Local<v8::Object> &containerObject, std::string field) {
    auto maybeFieldName = Nan::New(field);
    if (maybeFieldName.IsEmpty()) {
      v8::Local<v8::Value> emptyResult;
      return emptyResult;
    }

    auto maybeRetrievedField = Nan::Get(containerObject, maybeFieldName.ToLocalChecked());
    if (maybeRetrievedField.IsEmpty()) {
      v8::Local<v8::Value> emptyResult;
      return emptyResult;
    }

    return maybeRetrievedField.ToLocalChecked();
  }
}
