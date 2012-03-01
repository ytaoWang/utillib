/**
	min or max heap using list
	2011-6-22 23:21
 		Notice
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILTY or FITNESS FOR A PRATICULAR PURPOSE. See the GNU
	General Public License for more details.
**/

#include "heap.h"
#include <stdio.h>
 //change the address of pointer
 void _find_heap_node(struct heap_node **q,struct heap_root *root)
{
	int i,j;
	struct heap_node *p = root->root;
	struct heap_node node[root->length];
	 
	j=0;
	i=0;
	
	node[i++].left = p;
	
	while(j<i)
	{
	  p = node[j++].left;
	  
	  if(!p->left||!p->right)
	  {
		*q = p;
//		printf("finde q:%p,p:%p\n",q,p);
//		printf("find node:%ld\n",(*q)->value);
		return;
	  }
	  
	  node[i++].left = p->left;
	  node[i++].left = p->right;

	}
}

//just for test
void traverse_heap_node(struct heap_root *root)
{
	int i,j;
	struct heap_node node[root->length];
	struct heap_node *p,*q;
	
	i=0;
	j=0;
	
	node[i++].left = root->root;
	
	while(j<i)
	{
	  p = node[j++].left;
	  if(p->left)
		node[i++].left = p->left;
	  if(p->right)
		node[i++].left = p->right;
	  printf("traverse node:%ld\n",p->value);     
	}
}

static void _last_heap_node(struct heap_node **q,struct heap_root *root)
{
	int i,j;
	struct heap_node node[root->length];
	struct heap_node *p;
	
	i=0;
	j=0;
	
	node[i++].left = root->root;
	
	while(j<i)
	{
	  p = node[j++].left;
	  
	  if(p->left)
		node[i++].left = p->left;
	  if(p->right)
		node[i++].left = p->right;
//	  printf("traverse node:%ld\n",p->value);  
	  *q = p;   
	}
}

//maintain the heap property from parent
static void _adjust_parent(struct heap_root* root,struct heap_node *node)
{
	struct heap_node *p = node;
	struct heap_node *q = p->parent;
	
	while(p&&q)
	{
	//max heap
	if(root->flag&&q->value >= p->value) break;
	
	else    //min heap
		if(!root->flag&&q->value <= p->value) break;
	else   //adjust
	{
		
		exchange(p,q);
		p = q;
		q = q->parent;
	     }
	}
	
}
//maintain the heap property from the child
static void _adjust_child(struct heap_root *root,struct heap_node *node)
{
	struct heap_node *left = node->left;
	struct heap_node *right = node->right;
	struct heap_node *temp = node;
	
	//max heap property
	if(root->flag) {
	
	if(left && left->value > temp->value) temp = left;
	if(right && right->value > temp->value) temp = right;
	
	if(temp != node)
	{
	 exchange(temp,node);
	_adjust_child(root,temp);
	}
	
	}
	//min heap property
	else 
	{
	 if(left && left->value < temp->value ) temp = left;
	 if(right && right->value < temp->value) temp = right;
	 
	 if(temp != node)
	 {
	    exchange(temp,node);
	    _adjust_child(root,temp);
	 }
	} 
}
//replace node with key
int heap_key(struct heap_root *root,struct heap_node *node,long key)
{
	if(root->flag&&key<node->value) return -1;
	
	if(!root->flag&&key>node->value) return -1;
	
	node->value = key;
	_adjust_parent(root,node);
	
	return 0;
}

int heap_insert(struct heap_root *root,struct heap_node *node)
{
	struct heap_node *p;
		 
    if(!root->root && !root->length) {
        root->root = node;
        root->length = 1;
        return 0;
    }

	//find the fisrt node which is empty on left node or right
	_find_heap_node(&p,root);

//	printf("node value:%ld\n",p->value);
	
	if(p->left&&p->right) {
//		printf("find node:%p is not null,value:%ld\n",p,p->value);
		return -1;
	}

	if(!p->left) 
		p->left = node;
	else  if(!p->right) 
		p->right = node;

	node->parent = p;
	root->length +=1;
	_adjust_parent(root,node);

	return 0;		
}

struct heap_node* heap_extract(struct heap_root* root)
{
	struct heap_node *p,*q;
	
	p = NULL;
	q = NULL;

	if(root->length == 1)
	{
	   root->length = 0;
	   return root->root;
	}

	_last_heap_node(&p,root);
	 
//	if(p->right) p = p->right;
//	else if(p->left) p = p->left;
//	printf("last heap node:%ld\n",p->value);	
	exchange(p,root->root);
	
	q = p->parent;
		
	if(q->left == p)
	    q->left = NULL;
	else 
	    q->right = NULL;

	root->length -= 1;
	
//	printf("length:%d,root:%ld\n",root->length,(root->root).value);
	_adjust_child(root,root->root);
//	printf("after adjust:%ld\n",(root->root).value);
	return p;
	
}
