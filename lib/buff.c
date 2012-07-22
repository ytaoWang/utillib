#include <stdio.h>
#include <stdlib.h>

#include "buff.h"

static buff_t cur = NULL;

int buff_alloc(buff_t *pb)
{
    buff_t p;
    
    if(!(p = calloc(1,sizeof(struct buff))))
        return -1;
 
    list_init(&p->next);
    p->ref = 1;
    pb = p;
    
    if(!cur)
        cur = p;
    else 
        list_move(&cur->next,&p->next);
    
    return 0;
}

static void buff_free(buff_t *pb)
{
    if(pb->ptr) free(pb->ptr);
    
    list_del_element(&pb->next);

    if(cur == pb)
        cur = NULL;

    free(pb);
    pb = NULL;
}

void buff_dec(buff_t *pb)
{
    pb->ref--;
    if(!pb->ref)
        buff_free(pb);
}

int buff_fread(int fd,off_t offset,size_t count,\
               buff_t* _pb,uint64_t off)
{
    buff_t pb = *_pb;
    uint64_t size = count + off;
    int ret;
    
    if(pb->size < size) {
        if(!pb->size) {
            pb->ptr = calloc(size,sizeof(unsigned char));
            if(!pb->ptr) return -1;
        } else {
            //check reference
            if(pb->ref != 1) {
                if((buff_alloc(&pb)) < 0)
                    return -1;
                if(!(pb->ptr = calloc(size,sizeof(unsigned char)))) {
                    buff_dec(&pb);
                    return -1;
                }
            } else {
                if(pb->size < size) {
                    if(!(pb->ptr = realloc(pb,size)))
                        return -1; 
                }
            }
        }
    }
    
    ret = pread(fd,pb->ptr,count,offset);

    if((ret < 0) && (pb != *_pb)) {
        buff_dec(&pb);
        return ret;
    }

    pb->offset = size;
    //COW,it's written,change the buffer
    if(pb != *_pb) {
        buff_dec(_pb);
        *_pb = pb;
    }
    
    return ret;
}

int buff_frappend(int fd,off_t offset,size_t size,buff_t *_pb)
{
    return buff_fread(fd,offset,size,_pb,(*_pb)->offset);
}

int buff_fwrite(int fd,off_t offset,size_t size,buff_t pb,uint64_t off)
{
    return 0;
}


void buff_destroy(void)
{
    
}

