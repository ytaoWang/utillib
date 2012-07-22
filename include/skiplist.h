/**
 * @file   skiplist.h
 * @author wyt <wyt@wyt-laptop>
 * @date   Sun Jul 22 21:48:43 2012
 * 
 * @brief  skiplist implementation,
 *		   reference paper:Skip Lists:A probabilistic Alternative to Balanced Trees
 *		   MIT Lectures 12:Skip List
 * @email:datao0907@163.com
 * 
 */

#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_

typedef int (*skip_cmp)(const void *,const void *);
    
struct skip_item;

struct skiplist {
    struct skip_item **root;
    unsigned long level,cur;
    const skip_cmp *cmp;
};

struct skip_item {
    struct skip_item *next;
    struct skip_item *prev;
    struct skip_item *up;
    struct skip_item *down;
    void *data;
};

typedef struct skiplist * skiplist_t;

skiplist_t skiplist_init(unsigned long,const skip_cmp *);
int skiplist_insert(skiplist_t,void *);
void * skiplist_search(skiplist_t,const void *);
int skiplist_erase(skiplist_t,const void *);
void skiplist_destroy(skiplist_t);

#endif
