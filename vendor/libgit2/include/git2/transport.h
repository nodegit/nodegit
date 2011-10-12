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
#ifndef INCLUDE_git_transport_h__
#define INCLUDE_git_transport_h__

#include "common.h"
#include "types.h"
#include "net.h"

/**
 * @file git2/transport.h
 * @brief Git protocol transport abstraction
 * @defgroup git_transport Git protocol transport abstraction
 * @ingroup Git
 * @{
 */
GIT_BEGIN_DECL

/**
 * Get the appropriate transport for an URL.
 * @param tranport the transport for the url
 * @param url the url of the repo
 */
GIT_EXTERN(int) git_transport_new(git_transport **transport, const char *url);

/** @} */
GIT_END_DECL
#endif
