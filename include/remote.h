/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef REMOTE_H
#define REMOTE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class Remote : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_remote *GetValue();

    static Handle<Value> New(void *raw);

  private:
    Remote(git_remote *raw);
    ~Remote();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Create(const Arguments& args);
    static void CreateWork(uv_work_t* req);
    static void CreateAfterWork(uv_work_t* req);

    struct CreateBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_remote * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> nameReference;
      const char * name;
      Persistent<Value> urlReference;
      const char * url;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateInmemory(const Arguments& args);
    static Handle<Value> Load(const Arguments& args);
    static Handle<Value> Save(const Arguments& args);
    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Url(const Arguments& args);
    static Handle<Value> Pushurl(const Arguments& args);
    static Handle<Value> SetUrl(const Arguments& args);
    static Handle<Value> SetPushurl(const Arguments& args);
    static Handle<Value> SetFetchspec(const Arguments& args);
    static Handle<Value> Fetchspec(const Arguments& args);
    static Handle<Value> SetPushspec(const Arguments& args);
    static Handle<Value> Pushspec(const Arguments& args);
    static Handle<Value> Connect(const Arguments& args);
    static Handle<Value> Ls(const Arguments& args);
    static Handle<Value> Download(const Arguments& args);
    static Handle<Value> Connected(const Arguments& args);
    static Handle<Value> Stop(const Arguments& args);
    static Handle<Value> Disconnect(const Arguments& args);
    static Handle<Value> Free(const Arguments& args);
    static Handle<Value> UpdateTips(const Arguments& args);
    static Handle<Value> ValidUrl(const Arguments& args);
    static Handle<Value> SupportedUrl(const Arguments& args);
    static Handle<Value> List(const Arguments& args);
    static Handle<Value> CheckCert(const Arguments& args);
    static Handle<Value> SetCredAcquireCb(const Arguments& args);
    static Handle<Value> SetTransport(const Arguments& args);
    static Handle<Value> SetCallbacks(const Arguments& args);
    static Handle<Value> Stats(const Arguments& args);
    static Handle<Value> Autotag(const Arguments& args);
    static Handle<Value> SetAutotag(const Arguments& args);
    static Handle<Value> Rename(const Arguments& args);
    static Handle<Value> UpdateFetchhead(const Arguments& args);
    static Handle<Value> SetUpdateFetchhead(const Arguments& args);
    static Handle<Value> IsValidName(const Arguments& args);
    git_remote *raw;
};

#endif
