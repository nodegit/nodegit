#ifndef INCLUDE_refs_h__
#define INCLUDE_refs_h__

#include "common.h"
#include "git2/oid.h"
#include "git2/refs.h"
#include "hashtable.h"

#define GIT_REFS_DIR "refs/"
#define GIT_REFS_HEADS_DIR GIT_REFS_DIR "heads/"
#define GIT_REFS_TAGS_DIR GIT_REFS_DIR "tags/"
#define GIT_REFS_REMOTES_DIR GIT_REFS_DIR "remotes/"

#define GIT_RENAMED_REF_FILE GIT_REFS_DIR "RENAMED-REF"

#define GIT_SYMREF "ref: "
#define GIT_PACKEDREFS_FILE "packed-refs"
#define GIT_PACKEDREFS_HEADER "# pack-refs with: peeled "

#define GIT_HEAD_FILE "HEAD"
#define GIT_MERGE_HEAD_FILE "MERGE_HEAD"
#define GIT_REFS_HEADS_MASTER_FILE GIT_REFS_HEADS_DIR "master"

#define GIT_REFNAME_MAX 1024

struct git_reference {
	git_repository *owner;
	char *name;
	unsigned int type;
	time_t mtime;
};

typedef struct {
	git_hashtable *packfile;
	git_hashtable *loose_cache;
	time_t packfile_time;
} git_refcache;


void git_repository__refcache_free(git_refcache *refs);
int git_repository__refcache_init(git_refcache *refs);

int git_reference__normalize_name(char *buffer_out, size_t out_size, const char *name);
int git_reference__normalize_name_oid(char *buffer_out, size_t out_size, const char *name);

#endif
