#ifndef _HEAP_H_
#define _HEAP_H_

#include <limits.h>
#include <stdlib.h>

struct heap_node
{
	long value;
	struct heap_node *left;
	struct heap_node *right;
	struct heap_node *parent;
};

struct heap_root
{
	struct heap_node *root;
	unsigned int length;
	int flag:1;//max heap(1) min heap(0)
};

#define MAX_HEAP	1
#define MIN_HEAP	0

#define heap_node_init(ptr,_value)  ({\
	(ptr)->left = NULL;\
	(ptr)->right = NULL;\
	(ptr)->parent = NULL;\
	(ptr)->value = _value;})

#define heap_root_init(ptr,_flag,_value)  ({\
	heap_node_init((ptr)->root,_value);\
	ptr->length = _value?1:0;\
	ptr->flag = _flag;\
	})

#define heap_root_init_max(root) ({\
	heap_root_init(root,MAX_HEAP);\
	})

#define heap_root_init_min(root) ({\
	heap_root_init(root,MIN_HEAP);\
	})

static inline struct heap_node* heap_min(struct heap_root *root)
{
	if(!root->flag)
	   return root->root;	
}

static inline struct heap_node*  heap_max(struct heap_root *root)
{
	if(root->flag)
	    return root->root;
}

static inline int heap_length(struct heap_root *root)
{
	return root->length;
}

static inline void exchange(struct heap_node *node1,struct heap_node *node2)
{
	int temp;

	temp = node1->value;
	node1->value = node2->value;
	node2->value = temp;

}

//replace node(struct heap_node) with long(key)
int heap_key(struct heap_root*,
		struct heap_node*,long);

//insert a node into root
int heap_insert(struct heap_root*,
		struct heap_node*);

//extract the root node
struct heap_node* heap_extract(struct heap_root*);


#endif
