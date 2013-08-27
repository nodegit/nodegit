/**
 * This code is auto-generated; unless you know what you're doing, do not modify!
 **/

#ifndef GITREPO_H
#define GITREPO_H

#include <v8.h>
#include <node.h>
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

    static Handle<Value> New(const Arguments& args);


    static Handle<Value> Open(const Arguments& args);
    static void OpenWork(uv_work_t* req);
    static void OpenAfterWork(uv_work_t* req);

    struct OpenBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_repository * out;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> Init(const Arguments& args);
    static void InitWork(uv_work_t* req);
    static void InitAfterWork(uv_work_t* req);

    struct InitBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_repository * out;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Value> is_bareReference;
      unsigned is_bare;
      Persistent<Function> callback;
    };
    static Handle<Value> Path(const Arguments& args);
    static Handle<Value> Workdir(const Arguments& args);
    static Handle<Value> Odb(const Arguments& args);
    static Handle<Value> openIndex(const Arguments& args);
    static void openIndexWork(uv_work_t* req);
    static void openIndexAfterWork(uv_work_t* req);

    struct openIndexBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_index * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Function> callback;
    };
    static Handle<Value> GetBlob(const Arguments& args);
    static void GetBlobWork(uv_work_t* req);
    static void GetBlobAfterWork(uv_work_t* req);

    struct GetBlobBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_blob * blob;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> GetCommit(const Arguments& args);
    static void GetCommitWork(uv_work_t* req);
    static void GetCommitAfterWork(uv_work_t* req);

    struct GetCommitBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_commit * commit;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateCommit(const Arguments& args);
    static void CreateCommitWork(uv_work_t* req);
    static void CreateCommitAfterWork(uv_work_t* req);

    struct CreateCommitBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * id;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> update_refReference;
      const char * update_ref;
      Persistent<Value> authorReference;
      const git_signature * author;
      Persistent<Value> committerReference;
      const git_signature * committer;
      Persistent<Value> message_encodingReference;
      const char * message_encoding;
      Persistent<Value> messageReference;
      const char * message;
      Persistent<Value> treeReference;
      const git_tree * tree;
      Persistent<Value> parent_countReference;
      int parent_count;
      Persistent<Value> parentsReference;
      const git_commit ** parents;
      Persistent<Function> callback;
    };
    static Handle<Value> GetObject(const Arguments& args);
    static void GetObjectWork(uv_work_t* req);
    static void GetObjectAfterWork(uv_work_t* req);

    struct GetObjectBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_object * object;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Value> typeReference;
      git_otype type;
      Persistent<Function> callback;
    };
    static Handle<Value> GetReference(const Arguments& args);
    static void GetReferenceWork(uv_work_t* req);
    static void GetReferenceAfterWork(uv_work_t* req);

    struct GetReferenceBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_reference * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> nameReference;
      const char * name;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateSymbolicReference(const Arguments& args);
    static Handle<Value> CreateReference(const Arguments& args);
    static Handle<Value> AddRemote(const Arguments& args);
    static void AddRemoteWork(uv_work_t* req);
    static void AddRemoteAfterWork(uv_work_t* req);

    struct AddRemoteBaton {
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
    static Handle<Value> CreateRevWalk(const Arguments& args);
    static Handle<Value> GetSubmodule(const Arguments& args);
    static Handle<Value> AddSubmodule(const Arguments& args);
    static Handle<Value> GetTag(const Arguments& args);
    static void GetTagWork(uv_work_t* req);
    static void GetTagAfterWork(uv_work_t* req);

    struct GetTagBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_tag * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateTag(const Arguments& args);
    static void CreateTagWork(uv_work_t* req);
    static void CreateTagAfterWork(uv_work_t* req);

    struct CreateTagBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> tag_nameReference;
      const char * tag_name;
      Persistent<Value> targetReference;
      const git_object * target;
      Persistent<Value> taggerReference;
      const git_signature * tagger;
      Persistent<Value> messageReference;
      const char * message;
      Persistent<Value> forceReference;
      int force;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateLightweightTag(const Arguments& args);
    static void CreateLightweightTagWork(uv_work_t* req);
    static void CreateLightweightTagAfterWork(uv_work_t* req);

    struct CreateLightweightTagBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> tag_nameReference;
      const char * tag_name;
      Persistent<Value> targetReference;
      const git_object * target;
      Persistent<Value> forceReference;
      int force;
      Persistent<Function> callback;
    };
    static Handle<Value> GetTree(const Arguments& args);
    static void GetTreeWork(uv_work_t* req);
    static void GetTreeAfterWork(uv_work_t* req);

    struct GetTreeBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_tree * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> idReference;
      const git_oid * id;
      Persistent<Function> callback;
    };
    static Handle<Value> ReloadSubmodules(const Arguments& args);
    static void ReloadSubmodulesWork(uv_work_t* req);
    static void ReloadSubmodulesAfterWork(uv_work_t* req);

    struct ReloadSubmodulesBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Function> callback;
    };
    static Handle<Value> Delete(const Arguments& args);
    static void DeleteWork(uv_work_t* req);
    static void DeleteAfterWork(uv_work_t* req);

    struct DeleteBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> tag_nameReference;
      const char * tag_name;
      Persistent<Function> callback;
    };
    static Handle<Value> GetReferences(const Arguments& args);
    static void GetReferencesWork(uv_work_t* req);
    static void GetReferencesAfterWork(uv_work_t* req);

    struct GetReferencesBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_strarray * array;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> list_flagsReference;
      unsigned int list_flags;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateBlobFromBuffer(const Arguments& args);
    static void CreateBlobFromBufferWork(uv_work_t* req);
    static void CreateBlobFromBufferAfterWork(uv_work_t* req);

    struct CreateBlobFromBufferBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * oid;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> bufferReference;
      const void * buffer;
      Persistent<Value> lenReference;
      size_t len;
      Persistent<Function> callback;
    };
    static Handle<Value> CreateBlobFromFile(const Arguments& args);
    static void CreateBlobFromFileWork(uv_work_t* req);
    static void CreateBlobFromFileAfterWork(uv_work_t* req);

    struct CreateBlobFromFileBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_oid * id;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Value> pathReference;
      const char * path;
      Persistent<Function> callback;
    };
    static Handle<Value> GetRemotes(const Arguments& args);
    static void GetRemotesWork(uv_work_t* req);
    static void GetRemotesAfterWork(uv_work_t* req);

    struct GetRemotesBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_strarray * out;
      Persistent<Value> repoReference;
      git_repository * repo;
      Persistent<Function> callback;
    };
    static Handle<Value> Clone(const Arguments& args);
    static void CloneWork(uv_work_t* req);
    static void CloneAfterWork(uv_work_t* req);

    struct CloneBaton {
      uv_work_t request;
      int error_code;
      const git_error* error;
      git_repository * out;
      Persistent<Value> urlReference;
      const char * url;
      Persistent<Value> local_pathReference;
      const char * local_path;
      Persistent<Value> optionsReference;
      const git_clone_options * options;
      Persistent<Function> callback;
    };
    static Handle<Value> GetRemote(const Arguments& args);
    git_repository *raw;
};

#endif
