/*
 * =====================================================================================
 *
 *       Filename:  test_array.c
 *
 *    Description:  test zero -length array
 *
 *        Version:  1.0
 *        Created:  08/29/2011 10:54:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wyt (none), duanhancong@uestc.edu.cn
 *        Company:  ndsl
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "array.h"

struct name {
	int namelen;
	char name[0];
};

int cmp(void *s1,void *s2)
{
    return (*(int *)s1) - (*(int *)s2);
}


int main()
{
    array_t p;
    int a[] = {1,19,22,23,45,23,45,8};
    int elem,i;
    void *n;
    
    if(!(p = array_new())) {
        fprintf(stderr,"fail to allocate array");
        return -1;
    }

    array_pop(p);
    if(array_push(p,&a[0]) < 0) {
        fprintf(stderr,"fail to push array.\n");
        return -1;
    }
    
    
    n = array_get(p,100);
    fprintf(stderr,"n:%p,length:%d\n",n,array_size(p));
    n = array_get(p,0);
    fprintf(stderr,"n:%p\n",n);
    n = array_pop(p);
    fprintf(stderr,"n:%p,size:%d\n",n,*((int*)n));
    
    array_set(p,100,&a[1]);
    n = array_get(p,100);
    fprintf(stderr,"n:%p,length:%d,size:%d\n",n,array_size(p),*(int *)n);
    n = array_get(p,0);
    fprintf(stderr,"n:%p,length:%d\n",n,array_length(p));

    n = array_pop(p);
    fprintf(stderr,"n:%p,size:%d\n",n,*((int*)n));
    
    array_free(p);

    if(!(p = array_new())) {
        fprintf(stderr,"fail to allocate array");
        return -1;
    }

    int N = sizeof a / sizeof a[0];
    fprintf(stderr,"begin sort N:%d\n",N);
    for(i = 0;i < N;++i) {
        fprintf(stderr,"push %d\n",a[i]);
        array_push(p,&a[i]);
    }

    array_sort(p,cmp);
    int b = 100;
    
    n = array_find(p,&b,cmp);
    fprintf(stderr,"find n:%p,%d\n",n,(n?*((int *)n):0));
  
    for(i = 0; i < array_size(p);++i) {
        fprintf(stderr,"n:%d ",*((int *)array_get(p,i)));
    }
    
    
    while(array_size(p) > 0) {
        n = array_pop(p);
        fprintf(stderr,"n:%p,%d\n",n,*((int *)n));
    }
    
    
    array_free(p);
    
    
	return 0;
}
