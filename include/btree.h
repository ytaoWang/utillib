/**
 *	btree declaration 
 *  refernce by ItA:Introduction to Algorithm
 *  date:2011-12-17 19:49:56
 *  @author datao0907@163.com
 *
**/

#ifndef B_TREE_H_
#define B_TREE_H_

#define CHILD(t)  (2*(t))
#define KEY(t)    (2*(t)-1)
#define MIN_KEY(t) ((t)-1)
#define MIN_CHILD(t) (t)

#define LEAF      (1<<1)

#include <stdint.h>

typedef uintptr_t ptr_t;
typedef uint32_t u32;
typedef int (*compare_t)(const ptr_t,const ptr_t);

struct btree_root
{
  struct btree_node *root;
  compare_t com;
  int _degree;
  
};

struct btree_node 
{
  ptr_t *key;
  struct btree_node **child;
  struct btree_node *parent;
  uint32_t flag;
  uint32_t n;
};

#define INIT_ROOT(bt,_com,_degree)  {(bt)->root=NULL; (bt)->com=(_com);(bt)->_degree = _degree;}



struct btree_root *init_root(compare_t ,int);
int btree_insert(struct btree_root *,ptr_t);
int btree_full_delete(struct btree_root *,struct btree_node *,const ptr_t);
void btree_delete(struct btree_root *,struct btree_node *,const ptr_t);
void destry_root(struct btree_root *bt);
void btree_travel(struct btree_root *bt);


#endif
