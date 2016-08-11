#ifndef NODEGIT_H
#define NODEGIT_H

#include "thread_pool.h"

extern ThreadPool libgit2ThreadPool;

v8::Local<v8::Value> GetPrivate(v8::Local<v8::Object> object,
                                    v8::Local<v8::String> key);

void SetPrivate(v8::Local<v8::Object> object,
                    v8::Local<v8::String> key,
                    v8::Local<v8::Value> value);

#endif
