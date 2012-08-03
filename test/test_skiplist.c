#include <stdio.h>
#include <stdlib.h>

#include "skiplist.h"

static int compare(unsigned long key,unsigned long item)
{
    return key - item;
}


int main(int argc,char *argv[])
{
    skiplist_t sl;
    unsigned int value,i,max;
    
    if(argc != 2) {
        fprintf(stderr,"usage:%s num\n",argv[0]);
        return -1;
    }

    max = atoi(argv[1]);
    if(!(sl = skiplist_init(30,compare))) {
        fprintf(stderr,"fail to skiplist init\n");
        return -1;
    }
        
    fprintf(stderr,"skiplist:%p\n",sl);

    i = 0;
    while(i < max) {
        skiplist_insert(sl,i,i+1);
        i++;
    }

    i = 0;
    while(i < max) {
        value = skiplist_search(sl,i);
        printf("value:%d\n",value);
        i++;
    }
    skiplist_erase(sl,max - 2);
    
    skiplist_destroy(sl);

    return 0;
}
