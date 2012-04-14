#include "fib_heap.h"

#include <stdio.h>

#define N 2000

int main(int argc,char *argv[]) {
  
  int i;
  struct fh_node n[N];
  struct fh_root root;

  fh_init_root(&root);
  for(i = 0;i<N;i++) 
    fh_init_node(&n[i],i+1);
  for(i = 0;i<N;i++)
    fh_insert(&root,&n[i]);

  struct fh_node *p;
  for(i=0;i<N;i++)  {  
    p = fh_extract_min(&root);
    printf("i:%d,min:%ld\n",i,p->value);
  }
  
  return 0;
}
