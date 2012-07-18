/**
 *@file btree.c
 *@author datao0907
 *@brief implement B-tree operation
 *
 **/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "btree.h"

struct btree_root *init_root(compare_t com,int degree)
{
  struct btree_root *bt;
  
  if(degree < 2)
  {
    fprintf(stderr,"Btree's degree must be more than 2!\n");
    return NULL;
  }
  
  bt = (struct btree_root *)calloc(1,sizeof(struct btree_root));
  if(!bt)
  {
    fprintf(stderr,"cann't init root\n");
    return NULL;
  }

  bt->_degree = degree;
  bt->com = com;
  
  return bt; 
}

static void free_node(struct btree_node *);

void __destroy_root(struct btree_root *bt,struct btree_node *bn)
{
  int i;
  
  if(bn->flag & LEAF) //is a leaf,free it directly
  {
    free_node(bn);
    return;
  }

  for(i = 0;i< bn->n + 1;i++)
  {
    if(bn->child[i]) 
    {
      __destroy_root(bt,bn->child[i]);
    }
  }
  free_node(bn);
}


void destroy_root(struct btree_root *bt)
{
  struct btree_node *bn = bt->root;
  if(!bn)
  {  
    free(bt);
    return;
  }
  
  __destroy_root(bt,bn);
  free(bt);
}

static void free_node(struct btree_node *node)
{
  printf("free node :%p\n",node);
  free(node->key);
  free(node->child);
  free(node);
}

static struct btree_node * new_node(int _degree)
{
  struct btree_node *node;
  
  node = (struct btree_node *)calloc(1,sizeof(struct btree_node));
  if(!node) return NULL;
  
  node->key = (ptr_t*)calloc(KEY(_degree),sizeof(ptr_t));
  if(!node->key) 
  {
    free(node);
    return NULL;
  }

  node->child = (struct btree_node**)calloc(CHILD(_degree),\
                              sizeof(struct btree_node *));
  if(!node->child)
  {
    free(node->key);
    free(node);
    return NULL;
  }

  node->flag |= LEAF;
  node->n = 0;
  node->parent = NULL;
  
  return node;
}
void print(const struct btree_node *bn)
{
  int i;
  if(bn->flag & LEAF)
    printf("node(leaf %p)'s key:",bn);
  else
    printf("node(%p)'s key:",bn);

  for(i=0;i<bn->n;i++)
    printf(" %d ",bn->key[i]);
  
  printf("\n");
  
}

static int btree_split(struct btree_node *,int,struct btree_node *,int);

/**
 *@bt btree_root
 *@key the key you want to insert
 *@node insert node's root node
 */
static void btree_insert_nofull(struct btree_root *bt,ptr_t key,struct btree_node *node)
{
   int i;

   if(!node) node = bt->root;

   i = node->n-1;

   if(node->flag & LEAF) //node is a leaf,so insert into it
   {
     while(i>=0 && bt->com(node->key[i],key)>=0)
     {
       node->key[i+1] = node->key[i];
       i--;
     }

     node->key[i+1] = key;
     node->n ++;
     // printf("insert into btree_nofull key:%lu,length:%d\n",node->key[node->n-1],node->n);
   }
   else 
   {
     printf("node->key:%u,key:%u,i:%d\n",node->key[i],key,i);
     
     while(i>=0 && bt->com(node->key[i],key)>=0)
       i--;

     i++;
     //the place shoud be inserted into is the child i
     if(node->child[i] && node->child[i]->n == KEY(bt->_degree))
     {  
       btree_split(node,i,node->child[i],bt->_degree);
     //problem in here 
       i = 0;
       while(i<node->n && bt->com(node->key[i],key)<0)
         i++;
         //if(bt->com(node->key[i],key)<0)
     }
     if(i==node->n)
     printf("btree_insert not full:%d,node(%p)'s key(right):%d,\
i:%d\n",key,node,node->key[i-1],i);
     else
       printf("btree_insert not full:%d,node(%p)'s key(left):%d,\
i:%d\n",key,node,node->key[i],i);
     
    //    print(node);
    //print(node->child[i]);
    printf("insert node:%p\n",node->child[i]);
    btree_insert_nofull(bt,key,node->child[i]);
  }
}

/**
 *split the node into two part,first/left is node(t-1),
 *second/right is _new(t-1)
 *return median key in the _node(t)
 */
static int btree_split(struct btree_node *_node,int pos,\
                       struct btree_node *node,int _degree)
{
  int i;
  struct btree_node *_new;
  
  if(KEY(_degree) != node->n) return -1;
  if(!(_new = new_node(_degree))) return -1;
  
  i = _degree;
  while(i < KEY(_degree))
  {    
    _new->key[i-_degree] = node->key[i];
    _new->child[i-_degree] = node->child[i];
    node->key[i] = 0;
    node->child[i] = 0;
    i++;
  }

  _new->child[i-_degree] = node->child[i];
  node->child[i] = 0;
  
  _new->n = _degree -1;
  node->n = _degree -1;
  
  //insert into new node's child ptr
  i = _node->n;
  //first move place
  while(i > pos)
  {
    _node->key[i] = _node->key[i-1];
    _node->child[i+1] = _node->child[i];
    i--;
  }
  //insert new node's child ptr
  _node->child[pos+1] = _new;
  //insert median key into _node
  _node->key[pos] = node->key[_degree-1];
  _node->n++;
  node->key[_degree-1] = 0;

  node->parent= _node;
  _new->parent = _node;
  _node->flag &= ~LEAF;
  
  if(_new->child[0]) _new->flag &= ~LEAF;
  if(node->child[0]) node->flag &= ~LEAF;
  
  print(node);
  print(_new);
  print(_node);
  
  return 0;
}


int btree_insert(struct btree_root *bt,ptr_t key)
{
  struct btree_node *node = bt->root;
  
  if(!node) 
  {
    //node is leaf
    if(!(node = new_node(bt->_degree))) return -ENOMEM;
    bt->root = node;
  }
  
  printf("btree_insert key:%u\n",key);
  
  if(node->n == KEY(bt->_degree) ) //node is full,should split it  
  {
    struct btree_node *_node;
    if(!(_node = new_node(bt->_degree))) return -ENOMEM;
   
    _node->child[0] = node;
    _node->parent = node->parent;
    
    //split the node into two part(_node,node)
    btree_split(_node,0,node,bt->_degree);
    if(bt->root == node) bt->root = _node;
    //insert the key into the node whose parent is _node
    btree_insert_nofull(bt,key,_node);
  }
  else
    btree_insert_nofull(bt,key,node);
  
  return 0;
}
/**
 *@param bt btree_root
 *@param key search key
 *@param bnode:search the tree whose root is bnode
 *@param _node:return node contained key
 *
 *
*/
static int __btree_search(const struct btree_root *bt,const ptr_t key,struct btree_node *bnode,struct btree_node **_node)
{
  int i;
  
  if(!bt->root) return -ENOENT;
  
  if(!bnode) bnode = bt->root;
  
  i = bnode->n-1;
  
  if(bnode->flag & LEAF) //search node is a leaf node
  {
   
    //printf("bnode leaf key:%d\n",bnode->key[0]);
    
    while(i>=0 && bt->com(bnode->key[i],key)!=0)
      i--;
    
    if(i<0) {
      if(_node)
        *_node = NULL;
      return -ENOENT;
    }
    else 
    {
      if(_node) *_node = bnode;
      return 0;
    }
  }

  i = 0;
  
  while(i< bnode->n && bt->com(bnode->key[i],key)<0)
    i++;

  if(!bt->com(bnode->key[i],key)) 
  {
    if(_node) *_node = bnode;
    return 0;
  }
  
 return  __btree_search(bt,key,bnode->child[i],_node);
}


static void __replace(const struct btree_root *bt,struct btree_node *node,const ptr_t optr,const ptr_t nptr)
{
  int i;

  i=0;
  while(i < node->n && bt->com(node->key[i],optr)!=0)
    i++;
  
  if(i != node->n)
    node->key[i] = nptr;
}

int btree_search(const struct btree_root *bt,const ptr_t key,struct btree_node **_node)
{
  return __btree_search(bt,key,NULL,_node);
}
/**
 *@bt btree root
 *@node subroot at node
 *@p return predecessor node
 *@return predecessor'key
 *
 */
ptr_t btree_predecessor(struct btree_root *bt,struct btree_node *node,int index,\
                        struct btree_node **_node)
{
  struct btree_node *q;
  struct btree_node *p;
  
  q = node->child[index];
  //the rightmost child
  p = q->child[q->n];
  
  while(p)
  {
    q = p;
    p = p->child[p->n];
  }

  *_node = q;
  return q->key[q->n - 1];
}

ptr_t btree_successor(struct btree_root *bt,struct btree_node *node,int index,\
                      struct btree_node **_node)
{
  struct btree_node *q = node->child[index];
  struct btree_node *p;
  
  //the leftmost child
  p = q->child[0];
  while(p)
  {
    q = p;
    p = p->child[0];
  }
  
  *_node = q;
  
  return q->key[0];
}


//move index-th child into (index-1) child,merge
static void __move_left(struct btree_node *parent,int index,int clear)
{
  int i;
  struct btree_node *lc = parent->child[index - 1];
  struct btree_node *self = parent->child[index];
  
  //move parent key into left child
  lc->key[lc->n++] = parent->key[index - 1];

  //move self node into left child
  for(i = 0;i<self->n;i++)
  {  
    lc->key[i+lc->n] = self->key[i];
    lc->child[i+lc->n] = self->child[i];
    if(lc->child[i])
      lc->child[i+lc->n]->parent = lc;
  }
  
  lc->child[i+lc->n] = self->child[i];
  if(lc->child[i])
    lc->child[i+lc->n]->parent = lc;
  
  //update lc's number
  lc->n += self->n;
  //update self's number,clear data and child
  for(i = 0;i<self->n;i++)
  {
    self->child[i] = NULL;
    self->key[i] = 0;
  }
  self->child[i] = NULL;
  self->n = 0;
  
  if(!clear) return;
  //move parent key forward
  parent->child[index] = lc;
  for(i = index-1;i<parent->n;i++)
  {
    if(i!= parent->n - 1)
      parent->key[i] = parent->key[i+1];
    parent->child[i] = parent->child[i+1];
  }
  
  //printf("__move_left,self:%p,parent:%p,lc:%p,lc len:%d\n",self,parent,lc,lc->n);
  
  //update parent's number
  parent->n--;
}

//move the index-th child of parent and corresponding key into (index+1)th key
static void __move_right(struct btree_node *parent,int index,int clear)
{
  int i,len,j;
  struct btree_node *rc = NULL;
  struct btree_node *self;
  
  rc = parent->child[index+1];
  self = parent->child[index];
  
  len = rc->n;
  rc->n = rc->n + self->n + 1;
  //move right child backward 
  for(i=len,j=rc->n;i>0;i--,j--)
  {
    rc->key[j-1] = rc->key[i-1];
    rc->child[j] = rc->child[i];
  }
  rc->child[j] = rc->child[i]; 
  //move parent into rc
  rc->key[rc->n-len-1] = parent->key[index];
  //move self into rc
  for(i=0;i<self->n;i++)
  {
    rc->child[i] = self->child[i];
    rc->key[i] = self->key[i];
    if(rc->child[i])
      rc->child[i]->parent = rc;
    //clear self's child and key
    self->child[i] = NULL;
    self->key[i] = 0;
  }
  rc->child[i] = self->child[i];
  if(rc->child[i])
    rc->child[i]->parent=rc;
  //clear self child 
  self->child[i] = NULL;
  self->n = 0;
  
  if(!clear) return;
  //update parent key and child
  for(i=index;i<parent->n;i++)
  {
    if(i != parent->n - 1)
      parent->key[i] = parent->key[i+1];
    parent->child[i] = parent->child[i+1];
  }
  parent->n --;
  parent->key[parent->n] = 0;
  
  //  printf("__move_right,self:%p,parent:%p,rc:%p,rc len:%d\n",self,parent,rc,rc->n);
}

struct btree_node * __left_child(struct btree_root *bt,struct btree_node *node,ptr_t key)
{
  int i;

  i = 0;
  while(i<node->n && bt->com(node->key[i],key)<0)
    i++;
  if(i == 0) return NULL;
  return node->child[i];
}

struct btree_node * __right_child(struct btree_root *bt,struct btree_node *node,ptr_t key)
{
  int i;
  
  i = 0;
  while(i<node->n && bt->com(node->key[i],key)<0)
    i++;
  if(i == node->n) return NULL;
  return node->child[i+1];
}

static int __parent_index(struct btree_root *bt,struct btree_node *node,ptr_t key)
{
  int i ;
  i = 0;
  while(i<node->n && bt->com(node->key[i],key)<0) i++;
  if(i == 0) return i;
  return i-1;
}


void btree_delete(struct btree_root *bt,struct btree_node *_node,const ptr_t key)
{
  int i,j,key_index,sibling_index;
  struct btree_node *node,*p,*ls,*rs,*lc,*rc;
  int _degree;
  ptr_t tmp;

  node = NULL;
  p = NULL;
  ls = NULL;
  rs = NULL;
  lc = NULL;
  rc = NULL;
  
  
  node = bt->root;
  if(_node) node = _node;
  
  i = 0;
  _degree = bt->_degree;
  while(i<node->n && bt->com(node->key[i],key)<0) i++;
  key_index  = i;
  
  if(node->key[i] != key)
  {
    sibling_index = i;
    //key in the subtree rooted at the node->child[i]
    p = node->child[i];
    if(!node->parent) {
      printf("delete (in root)\n");
      print(p);
      
      btree_delete(bt,p,key);
      return;
    }
    //node's parent,so node's sibling
    ls = __left_child(bt,node->parent,node->key[node->n-1]);
    rs = __right_child(bt,node->parent,node->key[node->n-1]);
    

    if(node->n == MIN_KEY(_degree)) 
    {
      ptr_t parent_index;
      
      parent_index = __parent_index(bt,node->parent,node->key[node->n-1]);
      //borrow from left sibling
      if(!ls || ls->n == MIN_KEY(_degree)) goto _borrow_rs;
      
      printf("in borrow left sibling,ls:%p\n",ls);
      
      //move ls's rightmost node up into node
      tmp = ls->key[ls->n];
      rc = ls->child[ls->n];
      ls->child[ls->n] = NULL;
      ls->n --;

      //move backward
      node->n ++;
      i = node->n-1;
      node->child[i+1] = node->child[i];
      while(i > 0) 
      {
        node->key[i] = node->key[i-1];
        node->child[i] = node->child[i-1];
      }
      node->key[0] = node->parent->key[parent_index];
      node->child[0] = rc;
      node->parent->key[parent_index] = tmp;
      btree_delete(bt,p,key);
      return;
      
    _borrow_rs: //pass
      {
        if(!rs || rs->n == MIN_KEY(_degree)) goto _merge_all;
        
        printf("in borrow right sibling,rs:%p\n",rs);
        
        lc = rs->child[0];
        rs->child[0] = NULL;
        tmp = rs->key[0];
        print(rs);
        
        //move forward one step
        for(i = 0;i<rs->n;i++)
        {  
          if(i != rs->n-1)
            rs->key[i] = rs->key[i+1];
          rs->child[i] = rs->child[i+1];
        }
        rs->n--;
        
        node->key[node->n++] = node->parent->key[parent_index];
        node->child[node->n] = lc;
        node->parent->key[parent_index] = tmp;
        btree_delete(bt,p,key);
        return;
      }
    _merge_all:
      {
        printf("merge all index:%d in the node(%p)\n",i,node);
        i = 0;
        while(i<node->parent->n && bt->com(node->parent->key[i],key)<0) 
          i++;

        //merge right child and p
        if(rs)
        {
          __move_left(node->parent,i+1,1);
          free_node(rs);
        }
        else
        {
          __move_right(node->parent,i-1,1);
          free_node(ls);
        }
        if(node->parent->n == 0) 
        {
          if(bt->root == node->parent) bt->root = node;
          free_node(node->parent);
          node->parent = NULL;
        }
        btree_delete(bt,p,key);
        return;
      }
    }
    else
      btree_delete(bt,p,key);
  }
  else//key in the node
  {
    printf("delete key:%d of node:%p,len:%d\n",key,node,node->n);
    print(node);
    
    if(node->flag & LEAF) //it's in leaf
     {
       printf("delete in leaf,len:%d\n",node->n);
      if(node->n > MIN_KEY(_degree) || !node->parent)
      {
        printf("delete node leaf,key:%d,index:%d,length:%d\n",key,key_index,node->n);
        for(i=key_index;i<(node->n-1);i++)
          node->key[i] = node->key[i+1];
        
        node->n--;
        if(!node->parent && node->n == 0) {
          free_node(node);
          bt->root = NULL;
        }
        return;
      }
      p = node->parent;
      print(p);
      i = 0;
      while(i<p->n && bt->com(p->key[i],key)<0) i++;
      sibling_index = i;
      printf("sibling_index(LEAF):%d\n",sibling_index);
      if(sibling_index == 0) goto _borrow_l_rs;
      //borrow from left sibling
    _borrow_l_ls:
      {
        printf("borrow leaf left sibling,key:%d,i:%d\n",key,i); 
        ls = p->child[sibling_index-1];
        if(ls->n == MIN_KEY(_degree)) goto _merge_l_left;
        i = key_index;
        //move backward
        while(i>0)
          node->key[i] = node->key[i-1];
        node->key[i] = p->key[sibling_index];
        
        node->key[sibling_index-1] = ls->key[ls->n-1];
        ls->n--;
        
        return;
      }
    _merge_l_left:
      {
        printf("merge left leaf,key:%d,i:%d\n",key,i);
        __move_left(p,sibling_index,1);
        free_node(node);
        lc = p->child[sibling_index-1];
        if(p->n == 0)
        {
          if(bt->root == p) bt->root = lc;
          lc->parent = NULL;
          free_node(p);
        }
        
        btree_delete(bt,lc,key);
        return;
      }
      //borrow from right sibling
     _borrow_l_rs: //pass
      {
        printf("borrow leaf right sibling,key:%d,i:%d\n",key,sibling_index);
        rs = p->child[sibling_index+1];
        if(rs->n == MIN_KEY(_degree)) goto _merge_l_right;
        //move forward
        for(i = key_index;i<(node->n-1);i++)
          node->key[i] = node->key[i+1];
        node->key[i] = p->key[sibling_index];
        p->key[sibling_index] = rs->key[0];
        //move forward
        for(i = 0;i<(rs->n-1);i++)
          rs->key[i] = rs->key[i+1];
        rs->n--;
        return;
      }
      //merge with right
    _merge_l_right:
      {
        printf("merge right leaf,key:%d\n",key);
        __move_right(p,sibling_index,1);
        free_node(node);
        btree_delete(bt,p->child[sibling_index],key);
        if(p->n == 0)
        {
          if(bt->root == p) bt->root = p->child[sibling_index];
          p->child[sibling_index]->parent = NULL;
          free_node(p);
        }
        return;
      }
     }
    else //it's internal node
    {
      p = node;
      while(i<p->n && bt->com(p->key[i],key)<0) i++;
      sibling_index = i;
      printf("sibling_index(internal node):%d\n",sibling_index);
      //borrow from predecessor\
      //(left child's key more than min degree)
    _borrow_n_predecessor:
      {
        struct btree_node *_pre;
        printf("borrow node predecessor,key:%d\n",key);
        if(p->child[sibling_index]-> n == MIN_KEY(_degree)) 
          goto _borrow_n_successor;
        tmp = btree_predecessor(bt,node,sibling_index,&_pre);
        print(_pre);
        // if(_pre -> n == MIN_KEY(_degree)) goto _borrow_n_successor;
        btree_delete(bt,_pre,tmp);
        node->key[key_index] = tmp;
        return;
      }
      //borrow form successor
      //(right child's key more than min degree)
    _borrow_n_successor:
      {
        struct btree_node *next;
        printf("borrow node successor\n");
        if(p->child[sibling_index + 1]->n == MIN_KEY(_degree)) 
          goto _merge_n_rs;
        tmp = btree_successor(bt,node,sibling_index+1,&next);
        printf("successor tmp:%d,node len:%d\n",tmp,p->n);
        print(next);
        //if(next->n == MIN_KEY(_degree)) goto _merge_n_rs;
        btree_delete(bt,next,tmp);
        node->key[sibling_index] = tmp;
        printf("after delete tmp:%d\n",tmp);
        print(next);
        printf("after delete node\n");
        print(node);
        return;
      }
      //mege with righ child
    _merge_n_rs:
      {
        assert(sibling_index != p->n);
        
        printf("merge node right sibling,key:%d,sibling_index:%d\n",key,sibling_index);
        print(node);
        lc = p->child[sibling_index];
        __move_right(p,sibling_index,1);
        print(p->child[sibling_index]);
        free_node(lc);
        btree_delete(bt,p->child[sibling_index],key);
        if(p->n == 0)
        {
          if(bt->root == p) bt->root = p->child[sibling_index];
          p ->child[sibling_index]->parent = NULL;
          free_node(p);
        }
        return;
      }
    }
  }
  return;
}

int btree_full_delete(struct btree_root *bt,struct btree_node *_node,const ptr_t key)
{
  struct btree_node *node;
  int flag;
  
  while(!(flag = btree_search(bt,key,NULL)))
  { 
    printf("----start---full_delete(key:%d)\n",key);
    btree_delete(bt,_node,key);
    btree_travel(bt);
    printf("----end----full_delete(key:%d)\n",key);
  }
  
  if(flag != -ENOENT) return flag;
  return 0;
}

void btree_travel(struct btree_root *bt)
{
  struct btree_node *bn,*node;
  struct btree_node *list[65535];
  int i,j,k;
  
  bn = bt->root;
  if(!bn) return;
  
  i = 0;
  j = 0;
  k = 0;
  
  list[i++] = bn;
  while(i>j)
  {
    node = list[j++];
    printf("node(%p,len:%d)'s children:",node,node->n);
    k=0;

    while(k<node->n)
    {
      printf(" %d ",node->key[k]);
      k++;
    }
    
    if(!(node->flag & LEAF))
    {
      k = 0;
    while(k<node->n)
    {
      list[i++] = node->child[k];
      k++;
    }
    list[i++] = node->child[node->n];    
    }
    else printf("node is a leaf ");
    if(node->parent)
      printf("parent:%p,\n",node->parent);
    else 
      printf("parent is null\n");
    
  }
  
}
