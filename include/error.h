/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef ERROR_H
#define ERROR_H

#include <node.h>

#include "../vendor/libgit2/include/git2.h"

using namespace v8;
using namespace node;

/**
 * Class: GitError
 *   Wrapper for libgit2 git_error.
 */
class GitError : public ObjectWrap {
  public:
    /**
     * Variable: constructor_template
     *   Used to create Node.js constructor.
     */
    static Persistent<Function> constructor_template;
    /**
     * Function: Initialize
     *   Used to intialize the EventEmitter from Node.js
     *
     * Parameters:
     *   target - Object the Node.js global module object
     */
    static void Initialize(Handle<Object> target);

    static Local<Object> WrapError(const git_error* error);

  protected:
    const git_error* error;

    /**
     * Constructor: GitError
     */
    GitError() {};
    /**
     * Deconstructor: GitError
     */
    ~GitError() {};

    /**
     * Function: New
     *
     * Parameters:
     *   args Arguments function call
     *
     * Returns:
     *   Object args.This()
     */
    static Handle<Value> New(const Arguments& args);
};

#endif
