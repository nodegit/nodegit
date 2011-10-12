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

#include "common.h"
#include "git2/oid.h"
#include "repository.h"
#include <string.h>
#include <limits.h>

static signed char from_hex[] = {
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 00 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 10 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 20 */
 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, /* 30 */
-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 40 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 50 */
-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 60 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 70 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 80 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 90 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* a0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* b0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* c0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* d0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* e0 */
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* f0 */
};
static char to_hex[] = "0123456789abcdef";

int git_oid_fromstrn(git_oid *out, const char *str, size_t length)
{
	size_t p;

	if (length > GIT_OID_HEXSZ)
		length = GIT_OID_HEXSZ;

	if (length % 2)
		length--;

	for (p = 0; p < length; p += 2) {
		int v = (from_hex[(unsigned char)str[p + 0]] << 4)
		       | from_hex[(unsigned char)str[p + 1]];

		if (v < 0)
			return git__throw(GIT_ENOTOID, "Failed to generate sha1. Given string is not a valid sha1 hash");

		out->id[p / 2] = (unsigned char)v;
	}

	for (; p < GIT_OID_HEXSZ; p += 2)
		out->id[p / 2] = 0x0;

	return GIT_SUCCESS;
}

int git_oid_fromstr(git_oid *out, const char *str)
{
	return git_oid_fromstrn(out, str, GIT_OID_HEXSZ);
}

GIT_INLINE(char) *fmt_one(char *str, unsigned int val)
{
	*str++ = to_hex[val >> 4];
	*str++ = to_hex[val & 0xf];
	return str;
}

void git_oid_fmt(char *str, const git_oid *oid)
{
	size_t i;

	for (i = 0; i < sizeof(oid->id); i++)
		str = fmt_one(str, oid->id[i]);
}

void git_oid_pathfmt(char *str, const git_oid *oid)
{
	size_t i;

	str = fmt_one(str, oid->id[0]);
	*str++ = '/';
	for (i = 1; i < sizeof(oid->id); i++)
		str = fmt_one(str, oid->id[i]);
}

char *git_oid_allocfmt(const git_oid *oid)
{
	char *str = git__malloc(GIT_OID_HEXSZ + 1);
	if (!str)
		return NULL;
	git_oid_fmt(str, oid);
	str[GIT_OID_HEXSZ] = '\0';
	return str;
}

char *git_oid_to_string(char *out, size_t n, const git_oid *oid)
{
	char str[GIT_OID_HEXSZ];

	if (!out || n == 0 || !oid)
		return "";

	n--;  /* allow room for terminating NUL */

	if (n > 0) {
		git_oid_fmt(str, oid);
		if (n > GIT_OID_HEXSZ)
			n = GIT_OID_HEXSZ;
		memcpy(out, str, n);
	}

	out[n] = '\0';

	return out;
}

int git_oid__parse(git_oid *oid, const char **buffer_out,
		const char *buffer_end, const char *header)
{
	const size_t sha_len = GIT_OID_HEXSZ;
	const size_t header_len = strlen(header);

	const char *buffer = *buffer_out;

	if (buffer + (header_len + sha_len + 1) > buffer_end)
		return git__throw(GIT_EOBJCORRUPTED, "Failed to parse OID. Buffer too small");

	if (memcmp(buffer, header, header_len) != 0)
		return git__throw(GIT_EOBJCORRUPTED, "Failed to parse OID. Buffer and header do not match");

	if (buffer[header_len + sha_len] != '\n')
		return git__throw(GIT_EOBJCORRUPTED, "Failed to parse OID. Buffer not terminated correctly");

	if (git_oid_fromstr(oid, buffer + header_len) < GIT_SUCCESS)
		return git__throw(GIT_EOBJCORRUPTED, "Failed to parse OID. Failed to generate sha1");

	*buffer_out = buffer + (header_len + sha_len + 1);

	return GIT_SUCCESS;
}

void git_oid__writebuf(git_buf *buf, const char *header, const git_oid *oid)
{
	char hex_oid[GIT_OID_HEXSZ];

	git_oid_fmt(hex_oid, oid);
	git_buf_puts(buf, header);
	git_buf_put(buf, hex_oid, GIT_OID_HEXSZ);
	git_buf_putc(buf, '\n');
}

void git_oid_fromraw(git_oid *out, const unsigned char *raw)
{
	memcpy(out->id, raw, sizeof(out->id));
}

void git_oid_cpy(git_oid *out, const git_oid *src)
{
	memcpy(out->id, src->id, sizeof(out->id));
}

int git_oid_cmp(const git_oid *a, const git_oid *b)
{
	return memcmp(a->id, b->id, sizeof(a->id));
}

int git_oid_ncmp(const git_oid *oid_a, const git_oid *oid_b, unsigned int len)
{
	const unsigned char *a = oid_a->id;
	const unsigned char *b = oid_b->id;

	do {
		if (*a != *b)
			return 1;
		a++;
		b++;
		len -= 2;
	} while (len > 1);

	if (len)
		if ((*a ^ *b) & 0xf0)
			return 1;

	return 0;
}

typedef short node_index;

typedef union {
	const char *tail;
	node_index children[16];
} trie_node;

struct git_oid_shorten {
	trie_node *nodes;
	size_t node_count, size;
	int min_length, full;
};

static int resize_trie(git_oid_shorten *self, size_t new_size)
{
	self->nodes = realloc(self->nodes, new_size * sizeof(trie_node));
	if (self->nodes == NULL)
		return GIT_ENOMEM;

	if (new_size > self->size) {
		memset(&self->nodes[self->size], 0x0, (new_size - self->size) * sizeof(trie_node));
	}

	self->size = new_size;
	return GIT_SUCCESS;
}

static trie_node *push_leaf(git_oid_shorten *os, node_index idx, int push_at, const char *oid)
{
	trie_node *node, *leaf;
	node_index idx_leaf;

	if (os->node_count >= os->size) {
		if (resize_trie(os, os->size * 2) < GIT_SUCCESS)
			return NULL;
	}

	idx_leaf = (node_index)os->node_count++;

	if (os->node_count == SHRT_MAX)
		os->full = 1;

	node = &os->nodes[idx];
	node->children[push_at] = -idx_leaf;

	leaf = &os->nodes[idx_leaf];
	leaf->tail = oid;

	return node;
}

git_oid_shorten *git_oid_shorten_new(size_t min_length)
{
	git_oid_shorten *os;

	os = git__malloc(sizeof(git_oid_shorten));
	if (os == NULL)
		return NULL;

	memset(os, 0x0, sizeof(git_oid_shorten));

	if (resize_trie(os, 16) < GIT_SUCCESS) {
		free(os);
		return NULL;
	}

	os->node_count = 1;
	os->min_length = min_length;

	return os;
}

void git_oid_shorten_free(git_oid_shorten *os)
{
	free(os->nodes);
	free(os);
}


/*
 * What wizardry is this?
 *
 * This is just a memory-optimized trie: basically a very fancy
 * 16-ary tree, which is used to store the prefixes of the OID
 * strings.
 *
 * Read more: http://en.wikipedia.org/wiki/Trie
 *
 * Magic that happens in this method:
 *
 *	- Each node in the trie is an union, so it can work both as
 *	a normal node, or as a leaf.
 *
 *	- Each normal node points to 16 children (one for each possible
 *	character in the oid). This is *not* stored in an array of
 *	pointers, because in a 64-bit arch this would be sucking
 *	16*sizeof(void*) = 128 bytes of memory per node, which is fucking
 *	insane. What we do is store Node Indexes, and use these indexes
 *	to look up each node in the om->index array. These indexes are
 *	signed shorts, so this limits the amount of unique OIDs that
 *	fit in the structure to about 20000 (assuming a more or less uniform
 *	distribution).
 *
 *	- All the nodes in om->index array are stored contiguously in
 *	memory, and each of them is 32 bytes, so we fit 2x nodes per
 *	cache line. Convenient for speed.
 *
 *	- To differentiate the leafs from the normal nodes, we store all
 *	the indexes towards a leaf as a negative index (indexes to normal
 *	nodes are positives). When we find that one of the children for
 *	a node has a negative value, that means it's going to be a leaf.
 *	This reduces the amount of indexes we have by two, but also reduces
 *	the size of each node by 1-4 bytes (the amount we would need to
 *	add a `is_leaf` field): this is good because it allows the nodes
 *	to fit cleanly in cache lines.
 *
 *	- Once we reach an empty children, instead of continuing to insert
 *	new nodes for each remaining character of the OID, we store a pointer
 *	to the tail in the leaf; if the leaf is reached again, we turn it
 *	into a normal node and use the tail to create a new leaf.
 *
 *	This is a pretty good balance between performance and memory usage.
 */
int git_oid_shorten_add(git_oid_shorten *os, const char *text_oid)
{
	int i, is_leaf;
	node_index idx;

	if (os->full)
		return GIT_ENOMEM;

	if (text_oid == NULL)
		return os->min_length;

	idx = 0;
	is_leaf = 0;

	for (i = 0; i < GIT_OID_HEXSZ; ++i) {
		int c = from_hex[(int)text_oid[i]];
		trie_node *node;

		if (c == -1)
			return git__throw(GIT_ENOTOID, "Failed to shorten OID. Invalid hex value");

		node = &os->nodes[idx];

		if (is_leaf) {
			const char *tail;

			tail = node->tail;
			node->tail = NULL;

			node = push_leaf(os, idx, from_hex[(int)tail[0]], &tail[1]);
			if (node == NULL)
				return GIT_ENOMEM;
		}

		if (node->children[c] == 0) {
			if (push_leaf(os, idx, c, &text_oid[i + 1]) == NULL)
				return GIT_ENOMEM;
			break;
		}

		idx = node->children[c];
		is_leaf = 0;

		if (idx < 0) {
			node->children[c] = idx = -idx;
			is_leaf = 1;
		}
	}

	if (++i > os->min_length)
		os->min_length = i;

	return os->min_length;
}

