#include <stdio.h>
#include <stdlib.h>

#include "set.h"

struct set *set_new(void)
{
    struct set *ps;
    
    ps = calloc(1,sizeof(struct set));
    ps->parent = NULL;
    ps->child = NULL;
    ps->next = NULL;
    ps->rank = 0;
    
    return ps;
}

struct set *set_find(struct set *ps)
{
    if(ps->parent) 
        return set_find(ps->parent);
    return ps;
}

int set_union(struct set *ps1,struct set *ps2)
{
    struct set *p1,*p2;
    
    p1 = set_find(ps1);
    p2 = set_find(ps2);
    
    if(p1) {
        ps2->rank = p1->rank + 1;
        
    }
    
}

void set_free(struct set *ps)
{
    struct set *p,*q;
    
    p = ps->child;
    
    while(p) {
        q = p;
        set_free(q);
        p = p->next;
    }
    
}

