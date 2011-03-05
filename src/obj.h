/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#ifndef OBJ_H
#define OBJ_H

#include <v8.h>
#include <node.h>
#include <node_events.h>

#include <git2.h>

#include "repo.h"

using namespace node;

/**
 * Class wrapper for libgit2 git_object
 */
class Obj : public EventEmitter {
  public:
    /**
     * v8::FunctionTemplate used to create Node.js constructor
     */
    static Persistent<FunctionTemplate> constructor_template;

    /**
     * Used to intialize the EventEmitter from Node.js
     *
     * @param target v8::Object the Node.js module object
     */
    static void Initialize(Handle<v8::Object> target);

    /**
     * Accessor for Object
     *
     * @return the internal git_object reference
     */
    git_object* GetValue();

    /**
     * Mutator for Object
     *
     * @param obj a git_object object
     */
    void SetValue(git_object* obj);

  protected:
    /**
     * Constructor
     */
    Obj() {};

    /**
     * Deconstructor
     */
    ~Obj() {};

    /**
     * Mutator for Object
     *
     * @param args v8::Arguments function call arguments from Node.js
     *
     * @return v8::Object args.This()
     */
    static Handle<Value> New(const Arguments& args);

  private:
    /**
     * Internal reference to git_object object
     */
    git_object* obj;
};

#endif
