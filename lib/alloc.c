#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"

union obj {
    union ojb *link;
    char data[1];
};

#define __ALIGN 8
#define __MAX_BYTES 128
#define __NBLK (__ALIGN/__MAX_BYTES)

#define __ROUND_UP(x) (((x) + __ALIGN - 1) & ~(__ALIGN - 1))

static union obj *link[__NBLK];


static inline void *first_alloc(size_t nbytes)
{
    return calloc(nbytes,1);
}

static inline void first_dealloc(void *ptr)
{
    return free(ptr);
}

static void *first_realloc(void *ptr,size_t nbytes)
{
    void *result = realloc(ptr,nbytes);
    
    if(result) {
        memset(result,0,nbytes);
    }
    
    return result;
}

static void *second_alloc(size_t nbytes)
{
    
}

static void second_dealloc(size_t nbytes)
{
    
}

void *alloc(size_t n)
{
    if(__ROUND_UP(n) >= __MAX_BYTES)
        return firse_alloc(n);
    else
        return second_alloc(n);
}

void dealloc(size_t n)
{
    if(__ROUND_UP(n) >= __MAX_BYTES)
        return firse_dealloc(n);
    else
        return second_dealloc(n);
}
