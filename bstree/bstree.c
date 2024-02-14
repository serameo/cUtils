/*
File name: bstree.c
Author: Seree Meo Rakwong
Date: 01-FEB-2024
Purpose: Implement a simple binary search bst using qsort() and bsearch()
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstree.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define BSTREE_INIT_ITEMS          64
#define BSTREE_INCREMENT_ITEMS     64

#define BSTREE_ITEM_UNUSED         0
#define BSTREE_ITEM_USED           1
/*
reset data to nil, but NOT DELETE them
see also, bstree_clear()>>DELETE ALL DATA ALLOCATED BY bstree_set() or bstree_set_data()
*/
void bstree__reset_data(bstree_t* bst);
/*internal sorting*/
void bstree__sort(bstree_t* bst, int (cmp_f)(const void* p1, const void* p2));

struct bstree_item_s
{
    void*   data;
    int     used;   /*0-unused, 1-used*/
    struct bstree_s*    bst;
    int     index;  /*to tell us what it is at*/
};

#define BSTREE_ITEMS_SORTED         0
#define BSTREE_ITEMS_MODIFIED       1
struct bstree_s
{
    struct bstree_item_s*   items; /*array of items*/
    int     last_unused;         /*incrementer*/
    int     modified;
    int     nmaxitems;
    int     nlimits;
    int     nused;          /*count used items*/
    void*   cmp_userdata;   /*temporary pointer used by bstree_find()*/
    int(*cmp)(void* p1, void* p2, void* userdata);
};

int bstree__cmp_int(void* p1, void* p2, void* userdata)
{
    int i1 = (p1 ? *(int*)p1 : 0); /*find data*/
    int i2 = (p2 ? *(int*)p2 : 0); /*compare data (in array)*/
    if (!p2)
    {
        return 1;
    }
    return (i1 - i2);
}

void* bstree__new_item(const void* data, int sz1)
{
    void* newdata = 0;
    if (sz1 > 0 && data)
    {
        newdata = calloc(1, sz1);
        if (newdata)
        {
            memcpy(newdata, data, sz1);
        }
    }

    return newdata;
};

void bstree__set_parent(bstree_t* bst)
{
    int i = 0;
    for (i = 0; i < bst->nmaxitems; ++i)
    {
        bst->items[i].bst = bst;
        bst->items[i].index = i;
    }
}

bstree_t* bstree_new(int nitems, int nlimits)
{
    bstree_t* bst = (bstree_t*)calloc(1, sizeof(struct bstree_s));
    if (!bst)
    {
        return 0;
    }
    if (nitems < 1)
    {
        nitems = BSTREE_INIT_ITEMS;
    }
    bst->items = (struct bstree_item_s*)calloc(nitems, sizeof(struct bstree_item_s));
    if (!bst->items)
    {
        free(bst);
        return 0;
    }
    bst->last_unused = -1;
    bst->nmaxitems = nitems;
    bst->nlimits = nlimits;
    bst->modified = BSTREE_ITEMS_SORTED;
    bst->cmp = bstree__cmp_int;
    /*set all items having this pointer*/
    bstree__set_parent(bst);

    return bst;
}

void bstree_del(bstree_t* bst)
{
    if (bst)
    {
        bstree_clear(bst);
        free(bst->items);
        free(bst);
    }
}

void bstree_reset_item_data(bstree_t* bst, struct bstree_item_s* item)
{
    item->data  = 0;
    item->used  = 0;
    item->index = -1;
    bst->modified = BSTREE_ITEMS_MODIFIED;
}

void bstree__reset_data(bstree_t* bst)
{
    int i = 0;
    for (i = 0; i < bst->nmaxitems; ++i)
    {
        bst->items[i].data  = 0;
        bst->items[i].index = -1;
    }
    bst->modified = BSTREE_ITEMS_MODIFIED;
}
void bstree_clear(bstree_t* bst)
{
    int i = 0;
    for (i = 0; i < bst->nmaxitems; ++i)
    {
        bstree_set(bst, i, 0, 0);
    }
    bst->modified = BSTREE_ITEMS_SORTED;
    bst->nused = 0;
}

int bstree_foreach(bstree_t* bst, int(*foreach_f)(void* data, void* user), void* user)
{
    int i = 0;
    int rc = 0;
    struct bstree_item_s* item = 0;
    for (i = 0; i < bst->nmaxitems; ++i)
    {
        item = bstree_get(bst, i);
        if (item && !item->used)
        {
            continue;
        }
        rc = foreach_f(bstree_get_data(item), user);
        if (0 != rc)
        {
            break;
        }
    }
    return rc;
}

int bstree__compare(const void* p1, const void* p2)
{
    struct  bstree_item_s* q1 = (struct  bstree_item_s*)p1;
    struct  bstree_item_s* q2 = (struct  bstree_item_s*)p2;
    if (!q1->data || BSTREE_ITEM_UNUSED == q1->used)
    {
        return 1;
    }
    else if (!q2->data || BSTREE_ITEM_UNUSED == q2->used)
    {
        return -1;
    }
    return q1->bst->cmp(q1->data, q2->data, q1->bst->cmp_userdata);
}
/*internal sorting*/
void bstree__sort(bstree_t* bst, int (cmp_f)(const void* p1, const void* p2))
{
    struct bstree_item_s* items = 0;
    int nitems = bst->nused;
    int i = 0;
    int n = 0;
    if (0 == nitems)
    {
        return;
    }
    items = (struct bstree_item_s*)calloc(nitems, sizeof(struct bstree_item_s));
    if (!items)
    {
        return;
    }
    /*copy only used to items*/
    for (i = 0, n = 0; i < bst->nmaxitems; ++i)
    {
        if (bst->items[i].used)
        {
            /*items[n].data = bst->items[i].data;
            items[n].used = bst->items[i].used;
            items[n].bst = bst->items[i].bst;*/
            items[n] = bst->items[i];
            ++n;
        }
    }
    /*now, sort the items*/
    qsort(items, nitems, sizeof(struct bstree_item_s), cmp_f);
    /*ok, reset all the old items and replace with the new items*/
    bstree__reset_data(bst);
    /*copy*/
    memcpy(bst->items, items, (nitems * sizeof(struct bstree_item_s)));
    bst->nused = nitems;
    bst->modified = BSTREE_ITEMS_SORTED;
    bstree__set_parent(bst);/*re-index if need*/
    /*free the temp items*/
    free(items);
}

struct bstree_item_s* bstree_find(
    bstree_t* bst,
    void* finddata,
    int(*cmp)(void* p1, void* p2, void* userdata),
    void* userdata)
{
    struct bstree_item_s key;
    struct bstree_item_s* item = 0;
    /*assign temporary compare function and user data to bst object*/
    bst->cmp_userdata = userdata;
    bst->cmp = (cmp ? cmp : bstree__cmp_int);
    /*if bst is modified (adding, updating, deleting)*/
    if (bst->modified != BSTREE_ITEMS_SORTED)
    {
        /*qsort(bst->items, bst->nmaxitems, sizeof(struct bstree_item_s), bstree__compare);*/
        bstree__sort(bst, bstree__compare);
        /*bst->modified = BSTREE_ITEMS_SORTED;*/
    }
    key.data = finddata;
    key.bst = bst;
    key.used = BSTREE_ITEM_USED;

    item = bsearch(&key, bst->items, bst->nused, sizeof(struct bstree_item_s), bstree__compare);
    return item;
}

void* bstree_set_data(bstree_t* bst, struct bstree_item_s* item, void* data, int sz1)
{
    void* newdata = 0;

    if (!item || (item->data == data))
    {
        return 0;
    }
    if (!data)
    {
        /*clear*/
        free(item->data);
        item->data = 0;
        item->used = BSTREE_ITEM_UNUSED;
        --bst->nused;
    }
    else
    {
        newdata = bstree__new_item(data, sz1);
        if (!newdata)
        {
            return 0;
        }
        free(item->data);
        item->data = newdata;
        item->used = BSTREE_ITEM_USED;
        ++bst->nused;
    }
    bst->modified = BSTREE_ITEMS_MODIFIED;
    return newdata;
}
void* bstree_get_data(struct bstree_item_s* item)
{
    return (item ? item->data : 0);
}
int bstree_get_index(struct bstree_item_s* item)
{
    return (item ? item->index : -1);
}
struct bstree_item_s* bstree_get(bstree_t* bst, int at)
{
    if (at < 0 || at >= bst->nmaxitems)
    {
        return 0;
    }
    struct bstree_item_s* item = &bst->items[at];
    return item;
}

struct bstree_item_s* bstree_set(bstree_t* bst, int at, void* data, int sz1)
{
    struct bstree_item_s* item = bstree_get(bst, at);
    void* newdata = bstree_set_data(bst, item, data, sz1);
    return item;
}

struct  bstree_item_s* bstree_push_back(bstree_t* bst, void* data, int sz1)
{
    int last_unused = bst->last_unused;
    int i = (last_unused < 0 ? 0 : last_unused);
    int found_unused = 0;

    /*always scan an unused slot*/
    for (; i < bst->nmaxitems; ++i)
    {
        if (BSTREE_ITEM_UNUSED == bst->items[i].used)
        {
            found_unused = 1;
            last_unused = i;
            break;
        }
    }
    /*start again*/
    if (0 == found_unused)
    {
        for (i = 0; i < last_unused; ++i)
        {
            if (BSTREE_ITEM_UNUSED == bst->items[i].used)
            {
                found_unused = 1;
                last_unused = i;
                break;
            }
        }
    }
    if (0 == found_unused && (bst->nlimits > 0 && bst->nlimits < bst->nmaxitems))
    {
        /*try to re-allocate the new memory*/
        struct bstree_item_s* items = (struct bstree_item_s*)calloc(
            bst->nmaxitems + BSTREE_INCREMENT_ITEMS,
            sizeof(struct bstree_item_s));
        if (items)
        {
            /*copy the old items*/
            memcpy(items, bst->items, sizeof(struct bstree_item_s) * bst->nmaxitems);
            /*release the old items*/
            free(bst->items);
            bst->items = 0;
            /*set the new items*/
            bst->items = items;
            /*save the last unused*/
            last_unused = bst->nmaxitems;
            /*set the new max items*/
            bst->nmaxitems += BSTREE_INCREMENT_ITEMS;
            /*set all bst object pointer to the new items*/
            bstree__set_parent(bst);
        }
        else
        {
            /*insufficient memory*/
            return 0;
        }
    }
    return bstree_set(bst, last_unused, data, sz1);
}

int     bstree_count(bstree_t* bst)
{
    return bst->nused;
}


#ifdef __cplusplus
}
#endif
