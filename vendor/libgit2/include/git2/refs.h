/*
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * In addition to the permissions in the GNU General Public License,
 * the authors give you unlimited permission to link the compiled
 * version of this file into combinations with other programs,
 * and to distribute those combinations without any restriction
 * coming from the use of this file.  (The General Public License
 * restrictions do apply in other respects; for example, they cover
 * modification of the file, and distribution when not linked into
 * a combined executable.)
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDE_git_refs_h__
#define INCLUDE_git_refs_h__

#include "common.h"
#include "types.h"
#include "oid.h"

/**
 * @file git2/refs.h
 * @brief Git reference management routines
 * @defgroup git_reference Git reference management routines
 * @ingroup Git
 * @{
 */
GIT_BEGIN_DECL

/**
 * Lookup a reference by its name in a repository.
 *
 * The generated reference is owned by the repository and
 * should not be freed by the user.
 *
 * @param reference_out pointer to the looked-up reference
 * @param repo the repository to look up the reference
 * @param name the long name for the reference (e.g. HEAD, ref/heads/master, refs/tags/v0.1.0, ...)
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_lookup(git_reference **reference_out, git_repository *repo, const char *name);

/**
 * Create a new symbolic reference.
 *
 * The reference will be created in the repository and written
 * to the disk.
 *
 * This reference is owned by the repository and shall not
 * be free'd by the user.
 *
 * If `force` is true and there already exists a reference
 * with the same name, it will be overwritten.
 *
 * @param ref_out Pointer to the newly created reference
 * @param repo Repository where that reference will live
 * @param name The name of the reference
 * @param target The target of the reference
 * @param force Overwrite existing references
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_create_symbolic(git_reference **ref_out, git_repository *repo, const char *name, const char *target, int force);

/**
 * Create a new object id reference.
 *
 * The reference will be created in the repository and written
 * to the disk.
 *
 * This reference is owned by the repository and shall not
 * be free'd by the user.
 *
 * If `force` is true and there already exists a reference
 * with the same name, it will be overwritten.
 *
 * @param ref_out Pointer to the newly created reference
 * @param repo Repository where that reference will live
 * @param name The name of the reference
 * @param id The object id pointed to by the reference.
 * @param force Overwrite existing references
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_create_oid(git_reference **ref_out, git_repository *repo, const char *name, const git_oid *id, int force);

/**
 * Get the OID pointed to by a reference.
 *
 * Only available if the reference is direct (i.e. not symbolic)
 *
 * @param ref The reference
 * @return a pointer to the oid if available, NULL otherwise
 */
GIT_EXTERN(const git_oid *) git_reference_oid(git_reference *ref);

/**
 * Get full name to the reference pointed by this reference
 *
 * Only available if the reference is symbolic
 *
 * @param ref The reference
 * @return a pointer to the name if available, NULL otherwise
 */
GIT_EXTERN(const char *) git_reference_target(git_reference *ref);

/**
 * Get the type of a reference
 *
 * Either direct (GIT_REF_OID) or symbolic (GIT_REF_SYMBOLIC)
 *
 * @param ref The reference
 * @return the type
 */
GIT_EXTERN(git_rtype) git_reference_type(git_reference *ref);

/**
 * Get the full name of a reference
 *
 * @param ref The reference
 * @return the full name for the ref
 */
GIT_EXTERN(const char *) git_reference_name(git_reference *ref);

/**
 * Resolve a symbolic reference
 *
 * Thie method iteratively peels a symbolic reference
 * until it resolves to a direct reference to an OID.
 *
 * If a direct reference is passed as an argument,
 * that reference is returned immediately
 *
 * @param resolved_ref Pointer to the peeled reference
 * @param ref The reference
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_resolve(git_reference **resolved_ref, git_reference *ref);

/**
 * Get the repository where a reference resides
 *
 * @param ref The reference
 * @return a pointer to the repo
 */
GIT_EXTERN(git_repository *) git_reference_owner(git_reference *ref);

/**
 * Set the symbolic target of a reference.
 *
 * The reference must be a symbolic reference, otherwise
 * this method will fail.
 *
 * The reference will be automatically updated in
 * memory and on disk.
 *
 * @param ref The reference
 * @param target The new target for the reference
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_set_target(git_reference *ref, const char *target);

/**
 * Set the OID target of a reference.
 *
 * The reference must be a direct reference, otherwise
 * this method will fail.
 *
 * The reference will be automatically updated in
 * memory and on disk.
 *
 * @param ref The reference
 * @param id The new target OID for the reference
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_set_oid(git_reference *ref, const git_oid *id);

/**
 * Rename an existing reference
 *
 * This method works for both direct and symbolic references.
 * The new name will be checked for validity and may be
 * modified into a normalized form.
 *
 * The refernece will be immediately renamed in-memory
 * and on disk.
 *
 */
GIT_EXTERN(int) git_reference_rename(git_reference *ref, const char *new_name, int force);

/**
 * Delete an existing reference
 *
 * This method works for both direct and symbolic references.
 *
 * The reference will be immediately removed on disk and from
 * memory. The given reference pointer will no longer be valid.
 *
 */
GIT_EXTERN(int) git_reference_delete(git_reference *ref);

/**
 * Pack all the loose references in the repository
 *
 * This method will load into the cache all the loose
 * references on the repository and update the
 * `packed-refs` file with them.
 *
 * Once the `packed-refs` file has been written properly,
 * the loose references will be removed from disk.
 *
 * WARNING: calling this method may invalidate any existing
 * references previously loaded on the cache.
 *
 * @param repo Repository where the loose refs will be packed
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_packall(git_repository *repo);

/**
 * Fill a list with all the references that can be found
 * in a repository.
 *
 * The listed references may be filtered by type, or using
 * a bitwise OR of several types. Use the magic value
 * `GIT_REF_LISTALL` to obtain all references, including
 * packed ones.
 *
 * The string array will be filled with the names of all
 * references; these values are owned by the user and
 * should be free'd manually when no longer needed, using
 * `git_strarray_free`.
 *
 * @param array Pointer to a git_strarray structure where
 *		the reference names will be stored
 * @param repo Repository where to find the refs
 * @param list_flags Filtering flags for the reference
 *		listing.
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_listall(git_strarray *array, git_repository *repo, unsigned int list_flags);


/**
 * Perform an operation on each reference in the repository
 *
 * The processed references may be filtered by type, or using
 * a bitwise OR of several types. Use the magic value
 * `GIT_REF_LISTALL` to obtain all references, including
 * packed ones.
 *
 * The `callback` function will be called for each of the references
 * in the repository, and will receive the name of the reference and
 * the `payload` value passed to this method.
 *
 * @param repo Repository where to find the refs
 * @param list_flags Filtering flags for the reference
 *		listing.
 * @param callback Function which will be called for every listed ref
 * @param payload Additional data to pass to the callback
 * @return 0 on success; error code otherwise
 */
GIT_EXTERN(int) git_reference_foreach(git_repository *repo, unsigned int list_flags, int (*callback)(const char *, void *), void *payload);

/** @} */
GIT_END_DECL
#endif
