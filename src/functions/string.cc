#include <v8.h>
#include <string>

#include "../../include/functions/string.h"

std::string stringArgToString(const v8::Local<v8::Value> arg) {
    v8::String::Utf8Value param1(arg);
    return std::string(*param1);
}
