/*
File name: rbtree.c
Author: Seree Meo Rakwong
Date: 05-JAN-2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rbtree_s
{
    /*rbtree is a name of rb-tree*/
    RB_HEAD(rbtree, rbtree_node_s) head;  /*to allocate the tree header for each an instance*/
    rbtree_cmp_f       cmp;    /*to compare the rbtree_node_s->data when inserting/seaching/deleting*/
    rbtree_dup_f       dup;    /*to duplicate the data to rbtree_node_s->data*/
    rbtree_rel_f       rel;    /*to release the rbtree_node_s->data*/
};

struct rbtree_node_s
{
    RB_ENTRY(rbtree_node_s) entry;      /*TO BE HANDLED BY tree.h*/
    void*                   data;       /*user data to store and search*/
    struct rbtree_s*        rbtree;     /*pointer of the rbtree owner*/
};


/*this is the compare function to be called by tree.h*/
int rbtree_nodecmp(void* p1, void* p2)
{
    struct rbtree_node_s* n1 = (struct rbtree_node_s*)p1;
    struct rbtree_node_s* n2 = (struct rbtree_node_s*)p2;
    return n1->rbtree->cmp(n1->data, n2->data); /*called by the rbtree object owner*/
}
/*
generate rb-tree function for rbtree
e.g.
rbtree_RB_INSERT(x, y)
rbtree_RB_REMOVE(x, y)
rbtree_RB_FIND(x, y)
rbtree_RB_NFIND(x, y)
rbtree_RB_NEXT(y)
rbtree_RB_PREV(y)
rbtree_RB_MINMAX(x, RB_NEGINF)
rbtree_RB_MINMAX(x, RB_INF)
*/
RB_GENERATE(rbtree, rbtree_node_s, entry, rbtree_nodecmp)


int rbtree_cmp_int(void* p1, void* p2)
{
    int v1 = *(int*)p1;
    int v2 = *(int*)p2;
    return (v1 - v2);
}

int rbtree_cmp_string(void* p1, void* p2)
{
    char* v1 = (char*)p1;
    char* v2 = (char*)p2;
    return strcmp(v1, v2);
}

void* rbtree_dup(void* p1, int sz1)
{
    void* data = calloc(1, sz1);
    if (data)
    {
        memmove(data, p1, sz1);
    }
    return data;
}

void rbtree_rel(void* p1)
{
    free(p1);
}

/* implement functions*/
struct rbtree_node_s* rbtree_get_root(rbtree_t* rbtree)
{
    return RB_ROOT(&rbtree->head);
}

struct rbtree_node_s* rbtree_get_left(rbtree_t* rbtree, struct rbtree_node_s* n)
{
    return RB_LEFT(n, entry);
}

struct rbtree_node_s* rbtree_get_right(rbtree_t* rbtree, struct rbtree_node_s* n)
{
    return RB_RIGHT(n, entry);
}

struct rbtree_node_s* rbtree_get_next(rbtree_t* rbtree, struct rbtree_node_s* n)
{
    return RB_NEXT(rbtree, 0, n);
}

struct rbtree_node_s* rbtree_get_prev(rbtree_t* rbtree, struct rbtree_node_s* n)
{
    return RB_PREV(rbtree, 0, n);
}

struct rbtree_node_s* rbtree_get_min(rbtree_t* rbtree)
{
    return RB_MIN(rbtree, &rbtree->head);
}

struct rbtree_node_s* rbtree_get_max(rbtree_t* rbtree)
{
    return RB_MAX(rbtree, &rbtree->head);
}

void* rbtree_get_data(struct rbtree_node_s* n)
{
    return (n ? n->data : 0);
}

rbtree_t* rbtree_new(rbtree_cmp_f cmp, rbtree_dup_f dup, rbtree_rel_f rel)
{
    rbtree_t* rbtree = (rbtree_t*)calloc(1, sizeof(struct rbtree_s));
    if (rbtree)
    {
        if (!cmp)
        {
            rbtree->cmp = rbtree_cmp_int;
        }
        else
        {
            rbtree->cmp = cmp;
        }
        if (!dup)
        {
            rbtree->dup = rbtree_dup;
        }
        else
        {
            rbtree->dup = dup;
        }
        if (!rel)
        {
            rbtree->rel = rbtree_rel;
        }
        else
        {
            rbtree->rel = rel;
        }
    }
    return rbtree;
}

struct rbtree_node_s* rbtree_insert(rbtree_t* rbtree, void* p1, int sz1)
{
    struct rbtree_node_s* n1 = (struct rbtree_node_s*)calloc(1, sizeof(struct rbtree_node_s));
    if (n1)
    {
        n1->data    = rbtree->dup(p1, sz1);
        n1->rbtree  = rbtree;
        return RB_INSERT(rbtree, &rbtree->head, n1);
    }
    return 0;
}

struct rbtree_node_s* rbtree_insert_string(rbtree_t* rbtree, char* str1, int len1)
{
    return rbtree_insert(rbtree, str1, 1 + len1);
}

struct rbtree_node_s* rbtree_insert_int(rbtree_t* rbtree, int val1)
{
    return rbtree_insert(rbtree, &val1, sizeof(int));
}

struct rbtree_node_s* rbtree_find(rbtree_t* rbtree, void* p1)
{
    struct rbtree_node_s* find1;
    struct rbtree_node_s n1;
    n1.data     = p1;
    n1.rbtree   = rbtree;
    find1       = RB_FIND(rbtree, &rbtree->head, &n1);
    return find1;
}

struct rbtree_node_s* rbtree_find_int(rbtree_t* rbtree, int val1)
{
    return rbtree_find(rbtree, &val1);
}

struct rbtree_node_s* rbtree_find_string(rbtree_t* rbtree, char* str1)
{
    return rbtree_find(rbtree, str1);
}

struct rbtree_node_s* rbtree_nfind(rbtree_t* rbtree, void* p1)
{
    struct rbtree_node_s* nfind1;
    struct rbtree_node_s n1;
    n1.data     = p1;
    n1.rbtree   = rbtree;
    nfind1      = RB_NFIND(rbtree, &rbtree->head, &n1);
    return nfind1;
}

struct rbtree_node_s* rbtree_nfind_int(rbtree_t* rbtree, int val1)
{
    return rbtree_nfind(rbtree, &val1);
}

struct rbtree_node_s* rbtree_nfind_string(rbtree_t* rbtree, char* str1)
{
    return rbtree_nfind(rbtree, str1);
}

/*p1 may be a temp data to be deleted*/
void rbtree_remove(rbtree_t* rbtree, void* p1)
{
    struct rbtree_node_s n1;
    struct rbtree_node_s* n2;
    n1.data     = p1;
    n1.rbtree   = rbtree;
    n2          = RB_REMOVE(rbtree, &rbtree->head, &n1);
    if (n2)
    {
        rbtree->rel(n2->data);
        free(n2);
    }
}

void rbtree_remove_int(rbtree_t* rbtree, int val1)
{
    rbtree_remove(rbtree, &val1);
}

void rbtree_remove_string(rbtree_t* rbtree, char* str1)
{
    rbtree_remove(rbtree, str1);
}

void rbtree_remove_all(rbtree_t* rbtree)
{
    struct rbtree_node_s* root = rbtree_get_root(rbtree);
    while (root)
    {
        rbtree_remove(rbtree, root);
        root = rbtree_get_root(rbtree);
    }
}

void rbtree_foreach(rbtree_t* rbtree, int (*callback)(void* p1, void* userdata), void* userdata)
{
    struct rbtree_node_s* n1;
    if (callback)
    {
        RB_FOREACH(n1, rbtree, &rbtree->head)
        {
            /*user can always stop when callback() returned value not equal to zero*/
            if (0 != callback(n1, userdata))
            {
                break;
            }
        }
    }
}

void rbtree_update(rbtree_t* rbtree, struct rbtree_node_s* n1, void* p1, int sz1)
{
    void* data = 0;
    /*allocate the new data*/
    if (rbtree->dup)
    {
        data = rbtree->dup(p1, sz1);
    }
    else
    {
        data = rbtree_dup(p1, sz1);
    }
    /*if failed, do nothing*/
    if (!data)
    {
        return;
    }
    /*release the old data*/
    if (rbtree->rel)
    {
        rbtree->rel(n1->data);
    }
    else
    {
        rbtree_rel(n1->data);
    }
    /*assign the new data*/
    n1->data = data;
}
void rbtree_update_int(rbtree_t* rbtree, struct rbtree_node_s* n1, int p1)
{
    rbtree_update(rbtree, n1, &p1, sizeof(int));
}

void rbtree_update_string(rbtree_t* rbtree, struct rbtree_node_s* n1, char* p1)
{
    rbtree_update(rbtree, n1, p1, 1 + strlen(p1));
}


void rbtree_print(rbtree_t* rbtree, struct rbtree_node_s *n1, void (*print_f)(void* p1, int flag, void* userdata), void* userdata)
{
    struct rbtree_node_s *left, *right;
    if (!print_f)
    {
        return;
    }
    if (n1 == NULL)
    {
        print_f(0, RBTREE_NULL_NODE, userdata);
        return;
    }
    left = rbtree_get_left(rbtree, n1);
    right = rbtree_get_right(rbtree, n1);
    if (left == NULL && right == NULL)
    {
        print_f(n1->data, RBTREE_LEAF_NODE, userdata);
    }
    else
    {
        print_f(n1->data, RBTREE_NODE, userdata);
        print_f(0, RBTREE_LEFT_BEGIN, userdata);
        rbtree_print(rbtree, left, print_f, userdata);
        print_f(0, RBTREE_LEFT_END, userdata);
        print_f(0, RBTREE_RIGHT_BEGIN, userdata);
        rbtree_print(rbtree, right, print_f, userdata);
        print_f(0, RBTREE_RIGHT_END, userdata);
    }
}

#ifdef __cplusplus
}
#endif
