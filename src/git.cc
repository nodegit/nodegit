#include <git.h>


class Git2 : public ObjectWrap {
  public:
    static Persistent<ObjectTemplate> git; 
    static void Initialize (Handle<Object> target) {
      HandleScope scope;

      Local<Object> _ = Object::New();
      
      s_ct = Persistent<FunctionTemplate>::New(t);
      s_ct->InstanceTemplate()->SetInternalFieldCount(1);
      s_ct->SetClassName(String::NewSymbol("Git2"));

      //NODE_SET_PROTOTYPE_METHOD(s_ct, "git_repository_open", repository_open);
      //NODE_SET_PROTOTYPE_METHOD(s_ct, "git_repository_free", repository_free);
      //NODE_SET_PROTOTYPE_METHOD(s_ct, "git_strerror", strerror);

      target->Set(String::NewSymbol("Git2"), _);
    }

    int repository_open (const char* path) {
      return git_repository_open(&repo, path);
    }

    void repository_free (git_repository *repo) {
      git_repository_free(repo);
    }

    const char* strerror (int err) {
      return git_strerror(err);
    }
  
  protected:
    static Handle<Value> New (const Arguments& args) {
      HandleScope scope;

      Git2 *git2 = new Git2();
      git2->Wrap(args.This());

      return args.This();
    }

    struct async_repo {
      Git2 *git2;
      Persistent<Value> err;
      Persistent<Value> path;
      Persistent<Function> callback;
    };

    static Handle<Value> repository_open (const Arguments& args) {
      Git2 *git2 = ObjectWrap::Unwrap<Git2>(args.This());
      Local<Function> callback;

      HandleScope scope;

      // Ensure first param is a string
      if (args.Length() == 0 || !args[0]->IsString())
        return ThrowException(Exception::Error(String::New("Path is required and must be a String.")));

      // Ensure second param is a function if it exists
      if (args.Length() != 2 || !args[1]->IsFunction())
        return ThrowException(Exception::Error(String::New("Callback must be a Function.")));

      callback = Local<Function>::Cast(args[1]);

      async_repo *ar = new async_repo();
      ar->git2 = git2;
      ar->path = Persistent<Value>::New( args[0] );
      ar->callback = Persistent<Function>::New(callback);

      git2->Ref();

      // Place into EIO
      eio_custom(AsyncRepo, EIO_PRI_DEFAULT, AsyncRepoComplete, ar);
      ev_ref(EV_DEFAULT_UC);

      return Undefined();
    }

    static int AsyncRepo(eio_req *req) {
      async_repo *ar = static_cast<async_repo *>(req->data);

      String::Utf8Value path(ar->path);
      ar->err = Persistent<Value>::New( Integer::New(ar->git2->repository_open(*path)) );

      return 0;
    }

    static int AsyncRepoComplete(eio_req *req) {
      HandleScope scope;

      async_repo *ar = static_cast<async_repo *>(req->data);
      ev_unref(EV_DEFAULT_UC);
      ar->git2->Unref();

      Local<Value> argv[3];
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

    static Handle<Value> repository_free (const Arguments& args) {
      Git2 *git2 = ObjectWrap::Unwrap<Git2>(args.This());

      HandleScope scope;

      if (args.Length() == 0) {
        return ThrowException(
          Exception::Error(String::New("Repository required.")));
      }

      git_repository *repo = ObjectWrap::Unwrap<git_repository>(args[0]->ToObject());
      
      git2->repository_free(repo);
    }

    static Handle<Value> strerror (const Arguments& args) {
      Git2 *git2 = ObjectWrap::Unwrap<Git2>(args.This());

      HandleScope scope;

      if (args.Length() == 0 || !args[0]->IsNumber()) {
        return ThrowException(
          Exception::Error(String::New("Error code required.")));
      }

      Local<Integer> err = Local<Integer>::Cast( args[0] );
      
      return scope.Close(String::New(git2->strerror(err->Value())));
    }
};

extern "C" void init (Handle<Object> target) {
  HandleScope scope;

  Git2::Initialize(target);
}
