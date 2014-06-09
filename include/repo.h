/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREPO_H
#define GITREPO_H

#include <nan.h>
#include <string>

#include "git2.h"

using namespace node;
using namespace v8;

class GitRepo : public ObjectWrap {
  public:

    static Persistent<Function> constructor_template;
    static void Initialize (Handle<v8::Object> target);

    git_repository *GetValue();

    static Handle<Value> New(void *raw);

  private:
    GitRepo(git_repository *raw);
    ~GitRepo();

    static NAN_METHOD(New);


    struct OpenBaton {
      int error_code;
      const git_error* error;
      git_repository * out;
      const char * path;
    };
    class OpenWorker : public NanAsyncWorker {
      public:
        OpenWorker(
            OpenBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~OpenWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        OpenBaton *baton;
    };
    static NAN_METHOD(Open);

    struct InitBaton {
      int error_code;
      const git_error* error;
      git_repository * out;
      const char * path;
      unsigned is_bare;
    };
    class InitWorker : public NanAsyncWorker {
      public:
        InitWorker(
            InitBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~InitWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        InitBaton *baton;
    };
    static NAN_METHOD(Init);
    static NAN_METHOD(Path);
    static NAN_METHOD(Workdir);
    static NAN_METHOD(Odb);

    struct openIndexBaton {
      int error_code;
      const git_error* error;
      git_index * out;
      git_repository * repo;
    };
    class openIndexWorker : public NanAsyncWorker {
      public:
        openIndexWorker(
            openIndexBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~openIndexWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        openIndexBaton *baton;
    };
    static NAN_METHOD(openIndex);

    struct GetBlobBaton {
      int error_code;
      const git_error* error;
      git_blob * blob;
      git_repository * repo;
      const git_oid * id;
    };
    class GetBlobWorker : public NanAsyncWorker {
      public:
        GetBlobWorker(
            GetBlobBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetBlobWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetBlobBaton *baton;
    };
    static NAN_METHOD(GetBlob);

    struct GetCommitBaton {
      int error_code;
      const git_error* error;
      git_commit * commit;
      git_repository * repo;
      const git_oid * id;
    };
    class GetCommitWorker : public NanAsyncWorker {
      public:
        GetCommitWorker(
            GetCommitBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetCommitWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetCommitBaton *baton;
    };
    static NAN_METHOD(GetCommit);

    struct CreateCommitBaton {
      int error_code;
      const git_error* error;
      git_oid * id;
      git_repository * repo;
      const char * update_ref;
      const git_signature * author;
      const git_signature * committer;
      const char * message_encoding;
      const char * message;
      const git_tree * tree;
      int parent_count;
      const git_commit ** parents;
    };
    class CreateCommitWorker : public NanAsyncWorker {
      public:
        CreateCommitWorker(
            CreateCommitBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CreateCommitWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CreateCommitBaton *baton;
    };
    static NAN_METHOD(CreateCommit);

    struct GetObjectBaton {
      int error_code;
      const git_error* error;
      git_object * object;
      git_repository * repo;
      const git_oid * id;
      git_otype type;
    };
    class GetObjectWorker : public NanAsyncWorker {
      public:
        GetObjectWorker(
            GetObjectBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetObjectWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetObjectBaton *baton;
    };
    static NAN_METHOD(GetObject);

    struct GetReferenceBaton {
      int error_code;
      const git_error* error;
      git_reference * out;
      git_repository * repo;
      const char * name;
    };
    class GetReferenceWorker : public NanAsyncWorker {
      public:
        GetReferenceWorker(
            GetReferenceBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetReferenceWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetReferenceBaton *baton;
    };
    static NAN_METHOD(GetReference);
    static NAN_METHOD(CreateSymbolicReference);
    static NAN_METHOD(CreateReference);

    struct AddRemoteBaton {
      int error_code;
      const git_error* error;
      git_remote * out;
      git_repository * repo;
      const char * name;
      const char * url;
    };
    class AddRemoteWorker : public NanAsyncWorker {
      public:
        AddRemoteWorker(
            AddRemoteBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~AddRemoteWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        AddRemoteBaton *baton;
    };
    static NAN_METHOD(AddRemote);
    static NAN_METHOD(CreateRevWalk);
    static NAN_METHOD(GetSubmodule);
    static NAN_METHOD(AddSubmodule);

    struct GetTagBaton {
      int error_code;
      const git_error* error;
      git_tag * out;
      git_repository * repo;
      const git_oid * id;
    };
    class GetTagWorker : public NanAsyncWorker {
      public:
        GetTagWorker(
            GetTagBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetTagWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetTagBaton *baton;
    };
    static NAN_METHOD(GetTag);

    struct CreateTagBaton {
      int error_code;
      const git_error* error;
      git_oid * oid;
      git_repository * repo;
      const char * tag_name;
      const git_object * target;
      const git_signature * tagger;
      const char * message;
      int force;
    };
    class CreateTagWorker : public NanAsyncWorker {
      public:
        CreateTagWorker(
            CreateTagBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CreateTagWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CreateTagBaton *baton;
    };
    static NAN_METHOD(CreateTag);

    struct CreateLightweightTagBaton {
      int error_code;
      const git_error* error;
      git_oid * oid;
      git_repository * repo;
      const char * tag_name;
      const git_object * target;
      int force;
    };
    class CreateLightweightTagWorker : public NanAsyncWorker {
      public:
        CreateLightweightTagWorker(
            CreateLightweightTagBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CreateLightweightTagWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CreateLightweightTagBaton *baton;
    };
    static NAN_METHOD(CreateLightweightTag);

    struct GetTreeBaton {
      int error_code;
      const git_error* error;
      git_tree * out;
      git_repository * repo;
      const git_oid * id;
    };
    class GetTreeWorker : public NanAsyncWorker {
      public:
        GetTreeWorker(
            GetTreeBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetTreeWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetTreeBaton *baton;
    };
    static NAN_METHOD(GetTree);

    struct ReloadSubmodulesBaton {
      int error_code;
      const git_error* error;
      git_repository * repo;
    };
    class ReloadSubmodulesWorker : public NanAsyncWorker {
      public:
        ReloadSubmodulesWorker(
            ReloadSubmodulesBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~ReloadSubmodulesWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        ReloadSubmodulesBaton *baton;
    };
    static NAN_METHOD(ReloadSubmodules);

    struct DeleteBaton {
      int error_code;
      const git_error* error;
      git_repository * repo;
      const char * tag_name;
    };
    class DeleteWorker : public NanAsyncWorker {
      public:
        DeleteWorker(
            DeleteBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~DeleteWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        DeleteBaton *baton;
    };
    static NAN_METHOD(Delete);

    struct GetReferencesBaton {
      int error_code;
      const git_error* error;
      git_strarray * array;
      git_repository * repo;
      unsigned int list_flags;
    };
    class GetReferencesWorker : public NanAsyncWorker {
      public:
        GetReferencesWorker(
            GetReferencesBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetReferencesWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetReferencesBaton *baton;
    };
    static NAN_METHOD(GetReferences);

    struct CreateBlobFromBufferBaton {
      int error_code;
      const git_error* error;
      git_oid * oid;
      git_repository * repo;
      const void * buffer;
      size_t len;
    };
    class CreateBlobFromBufferWorker : public NanAsyncWorker {
      public:
        CreateBlobFromBufferWorker(
            CreateBlobFromBufferBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CreateBlobFromBufferWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CreateBlobFromBufferBaton *baton;
    };
    static NAN_METHOD(CreateBlobFromBuffer);

    struct CreateBlobFromFileBaton {
      int error_code;
      const git_error* error;
      git_oid * id;
      git_repository * repo;
      const char * path;
    };
    class CreateBlobFromFileWorker : public NanAsyncWorker {
      public:
        CreateBlobFromFileWorker(
            CreateBlobFromFileBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CreateBlobFromFileWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CreateBlobFromFileBaton *baton;
    };
    static NAN_METHOD(CreateBlobFromFile);

    struct GetRemotesBaton {
      int error_code;
      const git_error* error;
      git_strarray * out;
      git_repository * repo;
    };
    class GetRemotesWorker : public NanAsyncWorker {
      public:
        GetRemotesWorker(
            GetRemotesBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~GetRemotesWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        GetRemotesBaton *baton;
    };
    static NAN_METHOD(GetRemotes);

    struct CloneBaton {
      int error_code;
      const git_error* error;
      git_repository * out;
      const char * url;
      const char * local_path;
      const git_clone_options * options;
    };
    class CloneWorker : public NanAsyncWorker {
      public:
        CloneWorker(
            CloneBaton *_baton,
            NanCallback *callback
        ) : NanAsyncWorker(callback)
          , baton(_baton) {};
        ~CloneWorker() {};
        void Execute();
        void HandleOKCallback();

      private:
        CloneBaton *baton;
    };
    static NAN_METHOD(Clone);
    static NAN_METHOD(GetRemote);
    git_repository *raw;
};

#endif
