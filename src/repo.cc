#include "repo.h"

void Repo::Initialize (Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(Repo::New);
  
  Repo::s_ct = Persistent<FunctionTemplate>::New(t);
  Repo::s_ct->InstanceTemplate()->SetInternalFieldCount(1);
  Repo::s_ct->SetClassName(String::NewSymbol("Repo"));

  NODE_SET_PROTOTYPE_METHOD(Repo::s_ct, "open", Repo::open);
  //NODE_SET_PROTOTYPE_METHOD(s_ct, "free", free);

  target->Set(String::NewSymbol("Repo"), Repo::s_ct->GetFunction());
}

int Repo::open (const char* path) {
  return git_repository_open(&this->repo, path);
}

Handle<Value> Repo::New (const Arguments& args) {
  HandleScope scope;

  Repo *repo = new Repo();
  repo->Wrap(args.This());

  return args.This();
}

Handle<Value> Repo::open (const Arguments& args) {
  Repo *repo = ObjectWrap::Unwrap<Repo>(args.This());
  Local<Function> callback;

  HandleScope scope;

  if (args.Length() == 0 || !args[0]->IsString())
    return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));

  if (args.Length() != 2 || !args[1]->IsFunction())
    return ThrowException(Exception::Error(String::New("Callback must be a Function.")));

  callback = Local<Function>::Cast(args[1]);

  async_open *ar = new async_open();
  ar->repo = repo;
  ar->path = Persistent<Value>::New( args[0] );
  ar->callback = Persistent<Function>::New(callback);

  repo->Ref();

  eio_custom(AsyncOpen, EIO_PRI_DEFAULT, AsyncOpenComplete, ar);
  ev_ref(EV_DEFAULT_UC);

  return Undefined();
}

int Repo::AsyncOpen(eio_req *req) {
  async_open *ar = static_cast<async_open *>(req->data);

  String::Utf8Value path(ar->path);
  ar->err = Persistent<Value>::New( Integer::New(ar->repo->open(*path)) );

  return 0;
}

int Repo::AsyncOpenComplete(eio_req *req) {
  HandleScope scope;

  async_open *ar = static_cast<async_open *>(req->data);
  ev_unref(EV_DEFAULT_UC);
  ar->repo->Unref();

  Local<Value> argv[2];
  argv[0] = Number::Cast(*ar->err);
  argv[1] = String::Cast(*ar->path);

  TryCatch try_catch;

  ar->callback->Call(Context::GetCurrent()->Global(), 2, argv);

  if (try_catch.HasCaught())
    FatalException(try_catch);
    
  ar->err.Dispose();
  ar->callback.Dispose();
  ar->path.Dispose();

  delete ar;

  return 0;
}

extern "C" void init (Handle<Object> target) {
  HandleScope scope;

  Repo::Initialize(target);
}
