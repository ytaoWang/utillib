/**
 * @file   skiplist.c
 * @author wyt <wyt@wyt-laptop>
 * @date   Sun Jul 22 21:50:37 2012
 * 
 * @brief  skiplist implementation,see detail in skiplist.h
 * @email:datao0907@163.com
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "skiplist.h"

#define SKIPLIST_MAX_LEVEL 64
#define SKIPLIST_MIN_LEVEL 32
#define SKIPLIST_LEAST_LEVEL 1

static inline void skip_item_init(struct skip_item *item)
{
    item->next = item;
    item->prev = item;
    item->up = NULL;
    item->down = NULL;
    item->data = 0;
    item->key = 0;
}

static inline void skip_item_insert(struct skip_item *head,struct skip_item *item)
{
    struct skip_item *next = head->next;
    
    head->next = item;
    item->prev = head;
    item->next = next;
    
#ifdef SKIPLIST_DEBUG
    fprintf(stderr,"f:%s,head(key:%lu):%p,prev:%p,item:%p,(key:%lu,data:%lu),next:%p\n",\
            __FUNCTION__,head->key,head,item->prev,item,item->key,item->data,item->next);
#endif
    
    if(next)
        next->prev = item;
}

static unsigned int skiplist_level_generate(skiplist_t ptr)
{
    //random range in [1,ptr->cur] see book Number Recipes:The Art of Scientific Programming
    double level = ptr->level;
    
    srand(time(NULL));
    
    return 1 +(int)(rand() * level /(RAND_MAX+1.0));
}


static inline void skip_item_del(struct skip_item *item)
{
    struct skip_item *prev = item->prev;
    struct skip_item *next = item->next;
    
    prev->next = next;
    next->prev = prev;

    //emptize the item
    item->next = item;
    item->prev = item;
    item->down = NULL;
    item->up = NULL;
}

static inline void __skip_item_destroy(struct skip_item *item)
{
	#ifdef SKIPLIST_DEBUG
    fprintf(stderr,"line:%d,function:%s,prev:%p,it:%p(data:%lu,key:%lu),next:%p\n",\
            __LINE__,__FUNCTION__,item->prev,item,item->data,item->key,item->next);
	#endif
    skip_item_del(item);
    free(item);
}

skiplist_t skiplist_init(unsigned long level,skip_cmp cmp)
{
    skiplist_t ptr;
    int i;
    
    if(level < SKIPLIST_MIN_LEVEL)
        level = SKIPLIST_MIN_LEVEL;
    else if(level > SKIPLIST_MAX_LEVEL)
        level = SKIPLIST_MAX_LEVEL;
    
    if(!(ptr = calloc(1,sizeof(struct skiplist))))
        return NULL;
    
    ptr->cmp = cmp;
    ptr->cur = 0;
    ptr->level = level;
    
    if(!(ptr->root = calloc(level,sizeof(struct skip_item)))) {
        free(ptr);
        return NULL;
    }
    
    
    for(i = 0; i < level; ++i) {
        skip_item_init(&ptr->root[i]);
        if(i > 1)
            (ptr->root + i)->down = ptr->root + i-1;
        if(i < level - 1)
            (ptr->root + i)->up = ptr->root + i+1;
    }
    
    
    return ptr;
}

#define ITEM_FOUND 0
#define ITEM_NOT_FOUND 1

static int __skiplist_search(skiplist_t ps,unsigned long key,struct skip_item **it)
{
    struct skip_item *item,*next;
    int level;
    
    assert(ps);
    *it = &ps->root[ps->cur];

    if(!ps->cur)
        return ITEM_NOT_FOUND;

    next = (&ps->root[ps->cur - 1])->next;
    level = ps->cur - 1;
    
    while(next != &ps->root[level]) {
        item = next;

#ifdef SKIPLIST_DEBUG
        fprintf(stderr,"search item data:%lu,key:%lu\n",item->data,item->key);
#endif

        while((item != &ps->root[level]) && ps->cmp(key,item->key) > 0) {
            item = item->next;
#ifdef SKIPLIST_DEBUG
            fprintf(stderr,"l:%d,f:%s,lookup(%lu) item:%p, data:%lu,key:%lu\n", \
                    __LINE__,__FUNCTION__,key,item,item->data,item->key);
#endif
        }
        
        if((item != &ps->root[level]) && !ps->cmp(key,item->key)) {
            *it = item;
            return ITEM_FOUND;
        }
        
        next = item->prev;
        if(!next->down) {
            *it = next;
            return ITEM_NOT_FOUND;
        }
        next = next->down;
        level --;
    }
    
    *it = next;
    return ITEM_NOT_FOUND;
}

unsigned long skiplist_search(skiplist_t ps,unsigned long key)
{
    struct skip_item *item;

    if(__skiplist_search(ps,key,&item) == ITEM_NOT_FOUND)
        return 0;
    
    return item->data;
}

int skiplist_insert(skiplist_t ps,const unsigned long key,unsigned long data)
{
    int level,i;
    struct skip_item *it,*item;
    
    assert(ps);
    
    if((__skiplist_search(ps,key,&item)) == ITEM_FOUND)
        return -EEXIST;
    
    //get random level
    level = skiplist_level_generate(ps);
    struct skip_item *his[level];
    memset(his,0,sizeof(his));

    #ifdef SKIPLIST_DEBUG
    fprintf(stderr,"level:%d\n",level);
    #endif
    
    for(i = 0;i < level;i++) {
        it = calloc(1,sizeof(struct skip_item));
        if(!it) return -ENOMEM;
        skip_item_init(it);
        it->key = key;
        it->data = data;
        //insert into after item
        his[i] = it;
        skip_item_insert(item,it);

        while(!item->up)
            item = item->prev;
        item = item->up;
        
        if(i > 0) {
            it->down = his[i - 1];
            his[i-1]->up = it;
        }
        
#ifdef SKIPLIST_DEBUG
        fprintf(stderr,"line:%d,function:%s,level:%d,prev:%p,it:%p,next:%p,level:%i\n\n",__LINE__,__FUNCTION__,i,it->prev,it,it->next,i);
#endif    
    }
    
    if(ps->cur < level)
        ps->cur = level;
    
    return 0;
}

int skiplist_erase(skiplist_t ps,unsigned long key)
{
    struct skip_item *item,*it;
    int level;
    
    if((__skiplist_search(ps,key,&item)) == ITEM_NOT_FOUND)
        return -ENOENT;
    
    level = 0;
    
    #ifdef SKIPLIST_DEBUG
    fprintf(stderr,"line:%d,function:%s,key:%lu\n",__LINE__,__FUNCTION__,key);
    #endif
    
    while(item->up) {
        item = item->up;
        level ++;
    }
    
    if(level == ps->cur) {
        it = &ps->root[level];
        //item in the upper level
        if(it->next == item && item->next == it) {
            skip_item_init(ps->root + level);
            ps->level--;
        }
        
    }

    while(item) {
        it = item->down;
        __skip_item_destroy(item);
        item = it;
    }
    
    #ifdef SKIPLIST_DEBUG
    fprintf(stderr,"line:%d,function:%s,key:%lu\n",__LINE__,__FUNCTION__,key);
    #endif
    return 0;
}

void skiplist_destroy(skiplist_t ps)
{
    struct skip_item *it,*next;
    int i;
    
    for(i = 0; i < ps->cur;i++) {
        #ifdef SKIPLIST_DEBUG
        fprintf(stderr,"line:%d,function:%s,level:%d,p:%p\n",__LINE__,__FUNCTION__,i,ps->root+i);
        #endif
        it = (ps->root + i)->next;
        next = it->next;
        while(next && next != it) {
            next = it->next;
            #ifdef SKIPLIST_DEBUG
            fprintf(stderr,"it:%p,up:%p,down:%p\n",it,it->up,it->down);
            #endif
            __skip_item_destroy(it);
            it = next;
            next = next->next;
        }
    }
    
    free(ps->root);
    free(ps);
}
