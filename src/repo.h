#include <node.h>
#include <git2.h>

using namespace node;
using namespace v8;

class Repo : public ObjectWrap {
  public:
    static Persistent<FunctionTemplate> s_ct;
    static void Initialize (Handle<Object> target);
    int open(const char* path);
    Repo() {}
    ~Repo() {}
  protected:
    git_repository Value();
    static Handle<Value> New (const Arguments& args);
    static Handle<Value> open (const Arguments& args);
    static int AsyncOpen(eio_req *req);
    static int AsyncOpenComplete(eio_req *req);

    struct async_open {
      Repo *repo;
      Persistent<Value> err;
      Persistent<Value> path;
      Persistent<Function> callback;
    };
  private:
    git_repository *repo;
};
