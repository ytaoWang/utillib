#include "fib_heap.h"

#include <string.h>
#include <limits.h>

void fh_insert(struct fh_root *root,struct fh_node *node) {
  
  list_move(&root->root,&node->list);

  if(!root->min || fh_min(root) > node->value)
    root->min = node;

  root->num ++;
  #ifdef DEBUG
    printf("insert:%ld,num:%ld,min:%ld\n",node->value,root->num,root->min->value);
  #endif
}

static void fh_link(struct fh_node * d, struct fh_node *s) {
  
  list_del_element(&s->list);
  list_add_tail(&d->child,&s->list);

  d->degree++;
  fh_set_parent(s,d);
  fh_clear_mark(s);
  
  #ifdef DEBUG
  printf("link parent:%ld(%p),child:%ld(%p)\n",d->value,d,s->value,s);
  #endif
}

static void consolidate(struct fh_root *root) {
  
  struct fh_node *_pos,*pos,*tmp,*node,*n;
  struct fh_node *d[root->num];
  unsigned long degree;
  
  memset(d,0,root->num*sizeof(struct fh_node *));
  
  list_for_each_entry_del(pos,_pos,&root->root,list) {
    
    node = pos;
    degree = pos->degree;
    n = node;
    
    #ifdef DEBUG
    printf("consolidate list:%p,value:%ld\n",pos,pos->value);
    #endif

    while(d[degree]) {
      
      tmp = d[degree];

      #ifdef DEBUG
      printf("d[%ld]:%p,value:%ld,current(%p),value:%ld\n",\
             degree,d[degree],tmp->value,node,node->value);
      #endif

      if(tmp->value < node->value) {
        fh_link(tmp,node);
        n = tmp;
      } else {
        fh_link(node,tmp);
        n = node;
      }
      node = n;
      d[degree] = NULL;
      degree++;
    }
    
    d[degree] = n;
  }
  
  root->min = NULL;

  int i;
  for(i = 0;i < root->num;i++) {
    if(d[i]) {
      list_move(&root->root,&d[i]->list);
      if(!root->min || d[i]->value < fh_min(root))
        root->min = d[i];
    }
  }
  
  #ifdef DEBUG

  printf("after consolidate min value:%ld\n",fh_min(root));

  #endif
}


struct fh_node *fh_extract_min(struct fh_root *root) {
  
  struct fh_node *node,*pos,*_pos;
  
  node =  root->min;

  if(!node) return NULL;

  #ifdef DEBUG
  printf("extract_min:%ld\n",node->value);
  #endif

  list_for_each_entry_del(pos,_pos,&node->child,list) {
	
    list_move(&root->root,&pos->list);
    fh_clear_parent(pos);
  }

  
  if((root->num == 1) || list_empty(&node->list))
    root->min = NULL;
  else {
    root->min = fh_right(node);
    printf("itn't null:%p\n",root->min);
  }
  
  list_del_element(&node->list);
  root->num --;
  consolidate(root);

  return node;
}


static int cut(struct fh_root *root,struct fh_node *p,struct fh_node *c) {

  list_move(&root->root,&c->list);
  p->degree --;
  
  fh_clear_parent(c);
  fh_clear_mark(c);
}

static void cascading_cut(struct fh_root *root,struct fh_node *node) {
  
  struct fh_node *p;
  
  p = fh_parent(node);
  
  if(p) {
    if(!fh_get_mark(p)) 
      fh_set_mark(p);
    else {
      cut(root,p,node);
      cascading_cut(root,p);
    }
  }
}


static int fh_decrease_key(struct fh_root *root,struct fh_node *node,unsigned long key) {
  
  struct fh_node *p;
  
  if(node->value < key) return -1;
  
  node->value = key;
  p = fh_parent(node);
  
  if(p && p->value < node->value) {
    cut(root,p,node);
  }
  if(root->min && key < root->min->value)
    root->min = node;

  return 0;
}

int fh_earse(struct fh_root *root,struct fh_node *node) {
  
  int flag;
  
  if(!(flag = fh_decrease_key(root,node,0))) 
  {
    fh_extract_min(root);
  }
  
  return flag;  
}
