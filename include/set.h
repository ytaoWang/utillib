#ifndef __SET_H_
#define __SET_H_

struct set {
    unsigned int rank; //calculate rand bottom up
    struct set *parent;
    struct set *pre_child;
    struct set *next_child;
    struct set *pre_sibling;
    struct set *next_sibling;
};

struct set *set_new(void);
void set_union(struct set *,struct set *);
struct set *set_find(struct set *);
void set_free(struct set *);

#endif
