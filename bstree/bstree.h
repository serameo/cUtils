/*
File name: bstree.h
Author: Seree Meo Rakwong
Date: 10-JAN-2024
Purpose: Implement a simple binary search tree using qsort() and bsearch()
*/

#ifndef __BSTREE_H__
#define __BSTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

struct  bstree_item_s;  /*to be referenced*/
struct  bstree_s;
typedef struct bstree_s bstree_t;

bstree_t*  bstree_new(int nitems);
void    bstree_del(bstree_t* bst);

void* bstree_get(bstree_t* bst, int at);
void* bstree_set(bstree_t* bst, int at, void* data, int sz1);
/*
bstree_pushback() - add the new data if the bst object is full, 
                    bst will try adding more space to add the new data
                    if memory is insufficient, it will return NULL (0)
*/
struct  bstree_item_s* bstree_push_back(bstree_t* bst, void* data, int sz1);
void    bstree_clear(bstree_t* bst);
struct  bstree_item_s* bstree_find(
            bstree_t* bst,
            void* finddata,
            int (*cmp)(void* p1, void* p2, void* userdata),
            void* userdata);

#ifdef __cplusplus
}
#endif

#endif /* __BSTREE_H__ */

