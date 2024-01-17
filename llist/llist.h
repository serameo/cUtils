/*
File name: llist.h
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a doubly-linked list
*/

#ifndef __LLIST_H__
#define __LLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

struct llist_node_s;
struct llist_s;
typedef struct llist_s llist_t;


llist_t* llist_new();
void llist_del(llist_t* list);
void llist_remove(llist_t* list, struct llist_node_s* node);
void llist_remove_all(llist_t* list);
void llist_remove_first(llist_t* list);
void llist_remove_last(llist_t* list);
struct llist_node_s* llist_push_back(llist_t* list, void* data, int sz1);
struct llist_node_s* llist_push_front(llist_t* list, void* data, int sz1);
struct llist_node_s* llist_insert_sort(
    llist_t* list,
    void* data,
    int sz1,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata);
void llist_update(struct llist_node_s* node, void* data, int sz1);
struct llist_node_s* llist_find(
    llist_t* list,
    void* data,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata);
struct llist_node_s* llist_find_next(
    llist_t* list,
    struct llist_node_s* start);
struct llist_node_s* llist_rfind(
    llist_t* list,
    void* data,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata);
struct llist_node_s* llist_find_prev(
    llist_t* list,
    struct llist_node_s* start);


#ifdef __cplusplus
}
#endif


#endif /* __LLIST_H__*/
