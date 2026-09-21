#ifndef NODEGIT_H
#define NODEGIT_H

#include <napi.h>

Napi::Value GetPrivate(Napi::Object object,
                       Napi::String key);

void SetPrivate(Napi::Object object,
                Napi::String key,
                Napi::Value value);

#endif
