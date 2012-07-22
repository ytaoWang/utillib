#ifndef __ARRAY_H_
#define __ARRAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * vector -likely
 */

struct array {
    void **end;
    unsigned int cur;
    void **list;
};

typedef struct array* array_t;

array_t  array_new(void);
void* array_set(array_t,unsigned int,void *);
void* array_pop(array_t);
void* array_get(array_t,unsigned int);
int array_push(array_t, void *);
void array_free(array_t);
int array_size(array_t);
void array_sort(array_t,int (*cmp)(void *,void *));
void * array_find(array_t,void *,int (*cmp)(void *,void *));
    
#ifdef __cplusplus
}
#endif

#endif
