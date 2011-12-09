/*
 * Copyright 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
 * Dual licensed under the MIT and GPL licenses.
 */

#ifndef INDEX_H
#define INDEX_H

#include <node.h>

#include "../vendor/libgit2/include/git2.h"

using namespace node;

/**
 * Class: GitIndex
 *   Wrapper for libgit2 git_error.
 */
class GitIndex : public ObjectWrap {
  public:
    /**
     * Variable: constructor_template
     *   Used to create Node.js constructor.
     */
    static v8::Persistent<v8::FunctionTemplate> constructor_template;
    /**
     * Function: Initialize
     *   Used to intialize the EventEmitter from Node.js
     *
     * Parameters:
     *   target - v8::Object the Node.js global module object
     */
    static void Initialize(v8::Handle<v8::Object> target);

  protected:
    /**
     * Constructor: GitIndex
     */
    GitIndex() {};
    /**
     * Deconstructor: GitIndex
     */
    ~GitIndex() {};
    /**
     * Function: New
     *
     * Parameters:
     *   args v8::Arguments function call
     *
     * Returns:
     *   v8::Object args.This()
     */
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
};
 
#endif
