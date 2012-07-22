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

#include "skiplist.h"

static inline void skip_item_init(struct skip_item *item)
{
    item->next = item;
    item->prev = item;
    item->up = NULL;
    item->down = NULL;
    item->data = NULL;
}

static inline void skip_item_insert(struct skip_item *head,struct skip_item *item)
{
    struct skip_item *next = head->next;
    
    head->next = item;
    item->prev = head;
    item->next = next;
    
    if(next)
        next->prev = item;
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

static inline void skip_item_destroy(struct skip_item *item)
{
    skip_item_del(item);
    free(item);
}


#define SKIPLIST_MAX_LEVEL 64
#define SKIPLIST_MIN_LEVEL 32

skiplist_t skiplist_init(unsigned long level,const skip_cmp *cmp)
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
    
    if(!(ptr->root = calloc(level,sizeof(struct skip_item *)))) {
        free(ptr);
        return NULL;
    }
    
    
    for(i = 0; i < level; ++i) {
        skip_item_init(ptr->root+i);
        if(i > 1)
            ptr->root[i]->down = ptr->root[i-1];
        if(i < level - 1)
            ptr->root[i]->up = ptr->root[i+1];
    }
    
    
    return ptr;
}

#define ITEM_FOUND 0
#define ITEM_NOT_FOUND 1

static int skiplist_search(skiplist_t ps,const void *key,struct skip_item **it)
{
    struct skiplist_item *item,*next;
    
    assert(ps);
    next = ps->root[ps->cur];
    
    while(next) {
        item = next;
        while(item && ps->cmp(item->data,key) < 0)
            item = item->next;
        if(!ps->cmp(item->data,key)) {
            *it = item;
            return ITEM_FOUND;
        }
        
        next = item->prev;
        if(!next->item) { //level 0
            *it = next;
            return ITEM_NOT_FOUND;
        }
        
        next = next->down;
    }
    
    *it = next;
    return ITEM_NOT_FOUND;
}

void *skiplist_search(skiplist_t ps,const void *key)
{
    struct skip_item *item;

    if(skiplist_search(ps,key,&item) == ITEM_NOT_FOUND)
        return NULL;
    
    return item->data;
}

int skiplist_insert(skiplist_t ps,void *data)
{
    //get random level
    //then search into it
    //insert into
    return -ENOSYS;
}

int skiplist_erase(skiplist_t ps,void *data)
{
    struct skip_item *item,*it;
    int level;
    
    if((skiplist_search(ps,data,&item)) == ITEM_NOT_FOUND)
        return -ENOENT;
    
    level = 0;
    
    while(item->up) {
        item = item->up;
        level ++;
    }
    
    if(level == ps->cur) {
        it = ps->root[level];
        //item in the upper level
        if(it->next == item && item->next == it)
            ps->level--;
    }

    while(item) {
        it = item->down;
        skip_item_destroy(item);
        item = it;
    }
    
    return 0;
}
