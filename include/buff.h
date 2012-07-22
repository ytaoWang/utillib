#ifndef __BUFF_H_
#define __BUFF_H_

#include <stdint.h>

#include "list.h"

struct buff {
    int ref;
    uint64_t size;
    uint64_t offset;
    struct list next;
    void *ptr;
};

typedef struct buff * buff_t;

static inline uint32_t buff_size(const buff_t pb)
{
    return pb?pb->size:0;
}

static inline uint32_t buff_pos(const buff_t pb)
{
    return pb?pb->offset:0;
}

static inline void buff_inc(buff_t pb)
{
    pb->ref++;
}

static inline int buff_ref(buff_t pb)
{
    return pb->ref;
}

void buff_dec(buff_t);
int buff_alloc(buff_t *);
int buff_fread(int,off_t,size_t,buff_t *,uint64_t);
int buff_frappend(int,off_t,size_t,buff_t *);
int buff_fwrite(int,off_t,size_t,buff_t,uint64_t);
int buff_fwappend(int,off_t,size_t,buff_t *);

void buff_destroy(void);

#endif
