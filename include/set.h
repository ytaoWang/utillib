#ifndef __SET_H_
#define __SET_H_

struct set {
    unsigned int rank;
    struct set *parent;
    struct set *child;
    struct set *next;
};

struct set *set_new(void);
int set_union(struct set *,struct set *);
struct set *set_find(struct set *);
void set_free(struct set *);

#endif
