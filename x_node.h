#ifndef _X_NODE_H_
#define _X_NODE_H_
#include <stddef.h>
#include "list.h"
#include "hash.h"

#define NR_STATS 3 /* MOVEME */

#define X_HOST  0
#define X_JOB   1
#define X_CLUS  2
#define X_TOP_0 3
#define X_SERV  4
#define X_FS    5
#define X_TOP_1 6

#define X_TOP_0_NAME "ALL@ALL"
#define X_TOP_1_NAME "ALL"

#define K_TICK 10.0
#define K_WINDOW 600.0

struct x_node_ops {
  struct hash_table x_hash_table;
  size_t x_nr, x_nr_hint;
  int x_which;
};

struct x_node {
  struct x_node_ops *x_ops;
  struct x_node *x_parent;
  struct list_head x_parent_link;
  size_t x_nr_child;
  struct list_head x_child_list;
  struct list_head x_sub_list;
  size_t x_hash;
  struct hlist_node x_hash_node;
  char x_name[];
};

extern struct x_node_ops x_ops[];
extern struct x_node *x_top[2];

struct k_node {
  struct hlist_node k_hash_node;
  struct x_node *k_x[2];
  struct list_head k_sub_list;
  double k_t; /* Timestamp. */
  double k_pending[NR_STATS];
  double k_rate[NR_STATS]; /* EWMA bytes (or reqs) per second. */
  double k_sum[NR_STATS];
};

extern size_t nr_k;
extern struct hash_table k_hash_table;

int x_ops_init(void);
void x_init(struct x_node *x, int type, struct x_node *parent, size_t hash,
            struct hlist_head *hash_head, const char *name);
void x_set_parent(struct x_node *x, struct x_node *p);

struct x_node *x_lookup(int type, const char *name, int flags);

struct x_node *x_lookup_hash(int type, const char *name, size_t *hash_ref,
                             struct hlist_head **head_ref);

void x_update(struct x_node *x0, struct x_node *x1, double now, double *d);
void x_destroy(struct x_node *x);

static inline int x_which(struct x_node *x)
{
  return x->x_ops->x_which;
}

#define x_for_each_child(c, x)                          \
  list_for_each_entry(c, &((x)->x_child_list), x_parent_link)

#define x_for_each_child_safe(c, t, x)                          \
  list_for_each_entry_safe(c, t, &((x)->x_child_list), x_parent_link)

struct k_node *k_lookup(struct x_node *x0, struct x_node *x1, int flags);
void k_destroy(struct x_node *x0, struct x_node *x1, int which);
void k_update(struct k_node *k, struct x_node *x0, struct x_node *x1, double now, double *d);

#endif