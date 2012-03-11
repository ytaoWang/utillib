#ifndef __FIB_HEAP_H__
#define __FIB_HEAP_H__

#include "list.h"

struct fh_node;

struct fh_root {
  struct fh_node *min;
  struct list root;
  unsigned long num;
};

struct fh_node {
  unsigned long fh_parent_mark;
  struct list sibling;
  struct list child;
  struct list list; //for parent list
  unsigned long value;
  unsigned long degree;
} __attribute__((aligned(sizeof(long))));

#define fh_parent(r)    ((struct fh_node *)((r)->fh_parent_mark & ~3))  
#define fh_set_mark(r)  do { (r)->fh_parent_mark |= 1; } while(0)
#define fh_clear_mark(r)  do { (r)->fh_parent_mark &=~1; } while(0)
#define fh_get_mark(r)	  ((r)->fh_parent_mark |(~1))
#define fh_left(r)  (list_entry((r)->list.prev,struct fh_node,list))
#define fh_right(r) (list_entry((r)->list.next,struct fh_node,list))
#define fh_set_parent(r,p)  do { (r)->fh_parent_mark = ((p->fh_parent_mark)|((r)->fh_parent_mark & 3)); } while(0)

#define fh_clear_parent(r) do {(r)->fh_parent_mark &= 3; } while(0)

static inline unsigned long fh_min(struct fh_root *root) {
  
  return root->min?root->min->value:0;
}

static inline void fh_init_root(struct fh_root *root) {
  
  root->min = NULL;
  root->num = 0;
  list_init(&root->root);
}

static inline void fh_init_node(struct fh_node *node,unsigned long value) {

  node->fh_parent_mark = 0;
  list_init(&node->sibling);
  list_init(&node->child);
  list_init(&node->list);
  node->value = value;
  node->degree = 0;
}


struct fh_node *fh_extract_min(struct fh_root * root);
void fh_insert(struct fh_root *root,struct fh_node *node);
int fh_erase(struct fh_root *root,struct fh_node *node);

  

#endif
