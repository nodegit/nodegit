#include <node.h>
#include <node_events.h>
#include <git2.h>

using namespace node;
using namespace v8;

class Git2 : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> s_ct;
    static void Initialize (Handle<Object> target) {
      HandleScope scope;

      Local<FunctionTemplate> t = FunctionTemplate::New(New);
      
      s_ct = Persistent<FunctionTemplate>::New(t);
      s_ct->InstanceTemplate()->SetInternalFieldCount(1);
      s_ct->SetClassName(String::NewSymbol("Git2"));

      NODE_SET_PROTOTYPE_METHOD(s_ct, "repo", Repo);

      target->Set(String::NewSymbol("Git2"), s_ct->GetFunction());
    }

    Git2() {}
    ~Git2() {}

    ///home/tim/Dropbox/Projects/TabDeveloper/V4/.git
    int Repo (const char* path) {
      git_repository *repo;
      return git_repository_open(&repo, path);
    }
  
  protected:
    static Handle<Value> New (const Arguments& args) {
      HandleScope scope;

      Git2 *git2 = new Git2();
      git2->Wrap(args.This());

      return args.This();
    }

    static Handle<Value> Repo (const Arguments& args) {
      Git2 *git2 = ObjectWrap::Unwrap<Git2>(args.This());

      HandleScope scope;

      if (args.Length() == 0 || !args[0]->IsString()) {
        return ThrowException(
          Exception::Error(String::New("Repository path required.")));
      }

      String::Utf8Value path(args[0]->ToString());
      
      int err = git2->Repo(*path);

      if(err != 0) {
        return scope.Close(String::New(git_strerror(err)));
      }
      else {
        return scope.Close(String::New("Successfully loaded repo."));
      }
    }
};

Persistent<FunctionTemplate> Git2::s_ct;
extern "C" {
    static void init (Handle<Object> target) {
        Git2::Initialize(target);
    }

    NODE_MODULE(git2, init);
}
