#ifndef __ALLOC_H_
#define __ALLOC_H_

#ifdef __cplusplus
extern "C" 
{
#endif
    
void *alloc(size_t n);
void dealloc(void *);


#ifdef __cplusplus
}
#endif

#endif

