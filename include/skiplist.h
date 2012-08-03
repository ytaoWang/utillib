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

typedef int (*skip_cmp)(unsigned long,unsigned long);

#define SKIPLIST_END (unsigned long)-1   

struct skip_item;

struct skiplist {
    struct skip_item *root;
    unsigned long level;
    unsigned long cur;
    skip_cmp cmp;
};

struct skip_item {
    struct skip_item *next;
    struct skip_item *prev;
    struct skip_item *up;
    struct skip_item *down;
    unsigned long key;
    unsigned long data;
};

typedef struct skiplist * skiplist_t;

skiplist_t skiplist_init(unsigned long,skip_cmp);
int skiplist_insert(skiplist_t,unsigned long,unsigned long);
unsigned long skiplist_search(skiplist_t,unsigned long);
int skiplist_erase(skiplist_t,unsigned long);
void skiplist_destroy(skiplist_t);

#endif
