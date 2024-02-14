/*
File name: bstree.h
Author: Seree Meo Rakwong
Date: 01-FEB-2024
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
/*
bstree_new(): allocate the struct bstree_s
*/
bstree_t*  bstree_new(int nitems, int nlimits);
/*
bstree_del(): delete the allocated struct bstree_s
*/
void    bstree_del(bstree_t* bst);

/*
bstree_get(): get the struct bstree_item_s at the specific index
*/
struct bstree_item_s* bstree_get(bstree_t* bst, int at);
/*
bstree_set(): set the data to the struct bstree_item_s at the specific index
                by duplicating the new memory having the same size
if data is nil, the specific index will free the duplicated memory
*/
struct bstree_item_s* bstree_set(bstree_t* bst, int at, void* data, int sz1);
/*
bstree_set_data(): set the data to the struct bstree_item_s
if data is nil, the specific struct bstree_item_s will free the duplicated memory
*/
void* bstree_set_data(bstree_t* bst, struct bstree_item_s* item, void* data, int sz1);
/*
bstree_get_data(): get the data at the specific struct bstree_item_s
*/
void* bstree_get_data(struct bstree_item_s* item);
/*
bstree_get_index(): get the index of the specific struct bstree_item_s
*/
int bstree_get_index(struct bstree_item_s* item);
void bstree_reset_item_data(bstree_t* bst, struct bstree_item_s* item);
/*
bstree_pushback(): add the new data if the bst object is full,
bst will try adding more space to add the new data
if memory is insufficient, it will return NULL (0)
*/
struct  bstree_item_s* bstree_push_back(bstree_t* bst, void* data, int sz1);
/*
bstree_clear(): remove all allocated memory in the array of struct bstree_s
*/
void    bstree_clear(bstree_t* bst);
/*
bstree_find(): find the data matching with the finddata object
*/
struct bstree_item_s*   bstree_find(
                            bstree_t* bst,
                            void* finddata,
                            int(*cmp)(void* p1, void* p2, void* userdata),
                            void* userdata);
/*
bstree_foreach(): loop though all data in struct bstree_s object
*/
int     bstree_foreach(bstree_t* bst, int(*foreach_f)(void* data, void* user), void* user);
/*
bstree_count(): count the allocated data
*/
int     bstree_count(bstree_t* bst);
#ifdef __cplusplus
}
#endif

#endif /* __BSTREE_H__ */
