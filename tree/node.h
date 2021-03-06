/*
 * Copyright (c) 2012-2014 The nessDB Project Developers. All rights reserved.
 * Code is licensed with GPL. See COPYING.GPL file.
 *
 */

#ifndef nessDB_NODE_H_
#define nessDB_NODE_H_

#include "internal.h"
#include "posix.h"
#include "nmb.h"
#include "lmb.h"

#define PART_SIZE (sizeof(struct partition))
#define PIVOT_SIZE (sizeof(struct msg))

enum reactivity {
	FISSIBLE,
	FLUSHBLE,
	STABLE
};

struct ancestors {
	int level;
	int childnum;
	void *v;
	struct ancestors *next;
};

/* funcations for nodes operations */
struct node_operations {
	int (*update_func)(void *a, void *b);
	int (*delete_all_func)(void *a, void *b);
	int (*pivot_compare_func)(struct msg *a, struct msg *b);
};

struct partition {
	uint32_t inner_offset;		/* the postion start from the block start in disk */
	uint64_t child_nid;		/* child node id of this partition */
	struct nmb *buffer;
	int fetched: 2;			/* if 0, we need to fetch from disk */
	struct ness_mutex mtx;
	struct rwlock rwlock;
};

enum {LE_CLEAN = 0, LE_MVCC = 1};

struct node_attr {
	uint64_t newsz;
	uint64_t oldsz;
	ness_mutex_t mtx;
};

struct node {
	MSN msn;
	int dead;
	int dirty;
	int isroot;
	uint32_t height;
	uint64_t nid;
	struct timespec modified;
	struct node_attr attr;
	struct node_operations *node_op;
	enum lock_type pintype;
	struct cpair *cpair;

	union un {
		struct nonleaf {
			uint32_t n_children;		/* = pivots + 1 */
			struct msg *pivots;
			struct partition *parts;
		} n;
		struct leaf {
			uint8_t type;	/* LE_CLEAN or LE_MVCC */
			struct lmb *buffer;
			struct ness_mutex mtx;
			struct rwlock rwlock;
		} l;
	} u;
};

struct node *leaf_alloc_empty(NID nid);
void leaf_alloc_buffer(struct node *node);

struct node *nonleaf_alloc_empty(NID nid, uint32_t height, uint32_t children);
void nonleaf_alloc_buffer(struct node *node);

void node_set_dirty(struct node *n);
void node_set_nondirty(struct node *n);
int node_is_dirty(struct node *n);

uint32_t node_size(struct node *n);
uint32_t node_count(struct node *n);

int node_partition_idx(struct node *node, struct msg *k);
int node_find_heaviest_idx(struct node *node);

void node_free(struct node *n);

#endif /* nessDB_NODE_H_ */
