#include <stdio.h>
#include <stdlib.h>

#include "set.h"

static void __remove_self(struct set *s)
{
    struct set *prev,*next;
    
    prev = s->pre_sibling;
    next = s->next_sibling;
    
    if(prev)
        prev->next_sibling = next;
    if(next)
        next->pre_sibling = prev;
}

static void __add_child(struct set *parent,struct set *child)
{
    struct set *prev,*next;
    
    __remove_self(child);
    
    child->parent = parent;
    prev = parent->pre_child;
    next = parent->next_child;
    
    if(prev) 
        prev->next_sibling = child;
    child->pre_sibling = prev;
    
    if(next)
        next->pre_sibling = child;
    child->next_sibling = next;
}


static void set_link(struct set *ps1,struct set *ps2)
{
    
    if(ps1->rank > ps2->rank) {
        __add_child(ps1,ps2);
    } else {
        __add_child(ps2,ps1);
        if(ps1->rank == ps2->rank)
            ps2->rank ++;
    }
}


struct set *set_new(void)
{
    struct set *ps;
    
    ps = calloc(1,sizeof(struct set));
    ps->parent = NULL;
    ps->pre_child = NULL;
    ps->next_child = NULL;
    ps->pre_sibling = NULL;
    ps->next_sibling = NULL;
    ps->rank = 0;
    
    return ps;
}

struct set *set_find(struct set *ps)
{
    if(ps->parent) 
        return set_find(ps->parent);
    return ps;
}

void set_union(struct set *ps1,struct set *ps2)
{
    set_link(set_find(ps1),set_find(ps2));
}

void set_free(struct set *ps)
{
    struct set *p,*q;
    
    p = ps->next_child;
    
    while(p) {
        q = p;
        set_free(q);
        p = p->next_child;
    }
}

