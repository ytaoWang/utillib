#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

#define ARRAY_MIN 4
#define ARRAY_MAX 32
#define ARRAY_EXTEND 10

static void swap(void *v[],int i,int j)
{
    void *tmp;
    
    tmp = v[i];
    v[i] = v[j];
    v[j] = tmp;
}

array_t array_new(void)
{
    struct array *p;
    
    if(!(p = calloc(1,sizeof(struct array))))
        return NULL;
    
    
    if(!(p->list = calloc(ARRAY_MIN,sizeof(void *)))) {
        free(p);
        return NULL;
    }
    
    p->end = p->list + sizeof(void *) * ARRAY_MIN;
    p->cur = 0;

    return p;
}

int array_length(array_t p)
{
    return (p->end - p->list)/sizeof(void *);
}


int array_size(array_t p)
{
    return p->cur;
}

static int extend_array(array_t p,unsigned int min)
{
    
    if((min < p->cur) || (array_length(p) >min))
        return 0;
    
    min = min + ARRAY_EXTEND;
    
    #ifdef ARRAY_DEBUG
    fprintf(stderr,"%s,%d,min:%d,list:%p\n",__FUNCTION__,__LINE__,min,p->list);
    #endif
    
    void **q;
    if(!(q = calloc(min,sizeof(void *))))
        return -1;    

    memcpy(q,p->list,p->cur * sizeof(void *));

    free(p->list);

    p->list = q;
    p->end = p->list + sizeof(void *) * min;   
    
    #ifdef ARRAY_DEBUG
    fprintf(stderr,"%s,%d,min:%d,list:%p\n",__FUNCTION__,__LINE__,min,p->list);
    #endif
    
    return 0;
}


void* array_set(array_t p,unsigned int index,void *data)
{
    void *n;
    
    if((array_length(p) <= index) && (extend_array(p,index) < 0))
        return (void*)-1;
    
    n = p->list[index];
    
    if(p->cur <= index) {
        p->cur = index;
        p->list[p->cur++] = data;
    } else 
        p->list[index] = data;

    return n;
}

void *array_pop(array_t p)
{
    if(!p->cur)
        return NULL;
    
    return p->list[--p->cur];
}

void *array_get(array_t p,unsigned int index)
{
    if(p->cur < index)
        return NULL;
    
    return p->list[index];
}

int array_push(array_t p,void *data)
{
    if(((p->list + p->cur * sizeof(void *)) == p->end) && extend_array(p,p->cur) < 0)
        return -1;

#ifdef ARRAY_DEBUG
    fprintf(stderr,"cur:%d,length:%d,b:%p,t:%p,end:%p\n",\
            array_size(p),array_length(p),p->list,p->list + p->cur,p->end);
#endif

    p->list[p->cur++] = data;
    
    return 0;
}


static void array_qsort(void *v[],int left,int right,int (*cmp)(void *,void *))
{
    int i,last;
    
    if(left >= right)
        return;
    
    swap(v,left,(left+right)/2);
    last = left;
    
    for(i = left+1;i <= right; i++) 
        if((*cmp)(v[i],v[left]) < 0)
            swap(v,++last,i);
    swap(v,left,last);
    array_qsort(v,left,last-1,cmp);
    array_qsort(v,last+1,right,cmp);
}

void array_sort(array_t p,int (*cmp)(void *,void *))
{
    array_qsort(p->list,0,array_size(p) - 1,cmp);
}

void * array_find(array_t p ,void *data,int (*cmp)(void *,void *))
{
    int i;
    for(i = 0 ;i < array_size(p);++i)
        if(!(*cmp)(data,p->list[i]))
            return p->list[i];

    return NULL;
}


void array_free(array_t p)
{
    #ifdef ARRAY_DEBUG
    fprintf(stderr,"p:%p,list:%p\n",p,p->list);
    #endif
    if(p->list) {
        free(p->list);
        p->list = NULL;
    }
    
    free(p);
    
    p = NULL;
}
