#include <stdlib.h>
#include <stdio.h>

#include "btree.h"

#define LEN(key)  (sizeof(key)/sizeof(key[0]))

int compare(const ptr_t left,const ptr_t right)
{
  return (left - right);
}


int main(int argc,char *argv[])
{ 
  int i;
  struct btree_root *bt;
  struct btree_node *bn;
  
  ptr_t key[] = 
    {
      1,2,3,4,30,20,10,30,24,20,3,4,39
    };
  
  
  if(!(bt = init_root(compare,2)))
    return -1;
  printf("key len:%d\n",LEN(key));
  
  for(i=0;i<LEN(key);i++)
  {
    printf("-------begin(insert:%d)---------\n",key[i]);
    
  if(btree_insert(bt,key[i])<0) 
  {
    fprintf(stderr,"insert into btree error!\n");
    goto _destroy;
  }
  //  btree_travel(bt);
  printf("--------end(insert:%d)------\n",key[i]);
  
  }
  
  btree_travel(bt);
  /*
  if(btree_delete(bt,100)<0)
  {
    fprintf(stderr,"delete node key:%lu error\n",100UL);
    goto _destroy;
  }
  else fprintf(stderr,"delete node key:%lu successful\n",100UL);
  */
  for(i=0;i<LEN(key);i++)
  {
    printf("-----------begin(delete:%d,i:%d)----------\n",key[i],i);
    //if(btree_full_delete(bt,NULL,key[i])<0)
    btree_delete(bt,NULL,key[i]);
    /*
    {
      fprintf(stderr,"delete node key:%u error\n",key[i]);
      //goto _destroy;
    }
    else 
    {
      fprintf(stderr,"delete node key:%u succesful\n",key[i]);
    }
    */
    printf("--------travel------\n");
    btree_travel(bt);
    
    printf("-----------end(delete:%d)------------\n",key[i]);
  }
  
  /*
  if(btree_search(bt,100,&bn)<0)
  {
    fprintf(stderr,"cann't search the key:%ld\n",100L);
    goto _destroy;
  }
  else
  fprintf(stderr,"node length:%d,key:%lu\n",bn->n,bn->key[0]);
  */
  btree_travel(bt);
  
 _destroy:
  destroy_root(bt);
  return 0;
}
