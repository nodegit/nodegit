/**
 * Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * @author Michael Robinson @codeofinterest <mike@pagesofinterest.net>
 *
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef ERROR_H
#define ERROR_H

#include <node.h>

#include "git2.h"

using namespace v8;
using namespace node;

/**
 *   Wrapper for libgit2 git_error.
 */
class GitError : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;

    static void Initialize(Handle<Object> target);

    static Local<Object> WrapError(const git_error* error);

  protected:
    GitError() {};
    ~GitError() {};

    const git_error* error;

    static Handle<Value> New(const Arguments& args);
};

#endif
