/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREMOTE_H
#define GITREMOTE_H

#include <v8.h>
#include <node.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitRemote : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_remote *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRemote(git_remote *raw);
    ~GitRemote();

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Name(const Arguments& args);
    static Handle<Value> Url(const Arguments& args);
    static Handle<Value> PushUrl(const Arguments& args);
    static Handle<Value> SetUrl(const Arguments& args);
    static Handle<Value> SetPushUrl(const Arguments& args);
    static Handle<Value> Connect(const Arguments& args);
    static void ConnectWork(uv_work_t* req);
    static void ConnectAfterWork(uv_work_t* req);

    struct ConnectBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> remoteReference;
      git_remote * remote;
      Persistent<Value> directionReference;
      git_direction direction;
      Persistent<Function> callback;
    };
    static Handle<Value> Download(const Arguments& args);
    static void DownloadWork(uv_work_t* req);
    static void DownloadAfterWork(uv_work_t* req);

    struct DownloadBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> remoteReference;
      git_remote * remote;
      Persistent<Value> progress_cbReference;
      git_transfer_progress_callback progress_cb;
      Persistent<Value> payloadReference;
      void * payload;
      Persistent<Function> callback;
    };
    static Handle<Value> Connected(const Arguments& args);
    static Handle<Value> Stop(const Arguments& args);
    static Handle<Value> Disconnect(const Arguments& args);
    static void DisconnectWork(uv_work_t* req);
    static void DisconnectAfterWork(uv_work_t* req);

    struct DisconnectBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> remoteReference;
      git_remote * remote;
      Persistent<Function> callback;
    };
    static Handle<Value> UpdateTips(const Arguments& args);
    static Handle<Value> ValidUrl(const Arguments& args);
    static Handle<Value> SupportedUrl(const Arguments& args);
    static Handle<Value> CheckCert(const Arguments& args);
    static Handle<Value> UpdateFetchhead(const Arguments& args);
    static Handle<Value> SetUpdateFetchhead(const Arguments& args);
    static Handle<Value> IsValidName(const Arguments& args);
    git_remote *raw;
};

#endif
