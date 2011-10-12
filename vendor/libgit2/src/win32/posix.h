#ifndef INCLUDE_posix__w32_h__
#define INCLUDE_posix__w32_h__

#include "common.h"
#include "fnmatch.h"

GIT_INLINE(int) p_link(const char *GIT_UNUSED(old), const char *GIT_UNUSED(new))
{
	GIT_UNUSED_ARG(old)
	GIT_UNUSED_ARG(new)
	errno = ENOSYS;
	return -1;
}

GIT_INLINE(int) p_mkdir(const char *path, int GIT_UNUSED(mode))
{
	GIT_UNUSED_ARG(mode)
	return mkdir(path);
}

extern int p_unlink(const char *path);
extern int p_lstat(const char *file_name, struct stat *buf);
extern int p_readlink(const char *link, char *target, size_t target_len);
extern int p_hide_directory__w32(const char *path);
extern char *p_realpath(const char *orig_path, char *buffer);
extern int p_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
extern int p_snprintf(char *buffer, size_t count, const char *format, ...) GIT_FORMAT_PRINTF(3, 4);
extern int p_mkstemp(char *tmp_path);

#endif
