/*
File name: llist.c
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a doubly-linked list
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

struct llist_node_s
{
    struct llist_node_s* prev;
    struct llist_node_s* next;
    void*   data;
};

struct llist_s
{
    struct llist_node_s* first;
    struct llist_node_s* last;
    int nitems;
    /*temporary finding data*/
    int (*listcmp)(void* p1, void* p2, void* userdata);
    void* cmpdata;
    void* cmp_userdata;
};


llist_t* llist_new()
{
    llist_t* list = (struct llist_s*)calloc(1, sizeof(struct llist_s));
    if (!list)
    {
        return 0;
    }
    return list;
}

void llist_del(llist_t* list)
{
    if (list)
    {
        llist_remove_all(list);
        free(list);
    }
}

void llist_remove(llist_t* list, struct llist_node_s* node)
{
    struct llist_node_s* prev = 0;
    struct llist_node_s* next = 0;
    if (!node)
    {
        return;
    }
    prev = node->prev;
    next = node->next;
    if (node == list->first)
    {
        list->first->prev = 0;
        list->first = next;
    }
    else if (node == list->last)
    {
        list->last->next = 0;
        list->last = prev;
    }
    else
    {
        if (prev)
        {
            prev->next = next;
        }
        if (next)
        {
            next->prev = prev;
        }
    }
    node->prev = 0;
    node->next = 0;
    free(node->data);
    free(node);
    --list->nitems;
}

void llist_remove_all(llist_t* list)
{
    struct llist_node_s* first = list->first;
    while (first)
    {
        llist_remove_first(list);
        first = list->first;
    }
}

void llist_remove_first(llist_t* list)
{
    llist_remove(list, list->first);
}

void llist_remove_last(llist_t* list)
{
    llist_remove(list, list->last);
}

struct llist_node_s* llist__new_node(void* data, int sz1)
{
    struct llist_node_s* newnode = (struct llist_node_s*)calloc(1, sizeof(struct llist_node_s));
    if (!newnode)
    {
        return 0;
    }
    if (data && sz1 > 0)
    {
        newnode->data = calloc(1, sz1);
        if (newnode->data)
        {
            memcpy(newnode->data, data, sz1);
        }
    }
    return newnode;
}

struct llist_node_s* llist__insert(llist_t* list, struct llist_node_s* prev, struct llist_node_s* next, void* data, int sz1)
{
    struct llist_node_s* newnode = llist__new_node(data, sz1);
    if (!newnode)
    {
        return 0;
    }
    if (!list->first)
    {
        list->first = list->last = newnode;
        ++list->nitems;
        return newnode;
    }
    if (prev)
    {
        newnode->prev = prev;
        prev->next = newnode;
    }
    if (next)
    {
        newnode->next = next;
        next->prev = newnode;
    }
    if (next == list->first)
    {
        list->first = newnode;
    }
    else if (prev == list->last)
    {
        list->last = newnode;
    }
    ++list->nitems;
    return newnode;
}

struct llist_node_s* llist_push_back(llist_t* list, void* data, int sz1)
{
    return llist__insert(list, list->last, 0, data, sz1);
}

struct llist_node_s* llist_push_front(llist_t* list, void* data, int sz1)
{
    return llist__insert(list, 0, list->first, data, sz1);
}

struct llist_node_s* llist_insert_sort(
    llist_t* list,
    void* data,
    int sz1,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata)
{
    struct llist_node_s* curnode = 0;
    /*default push back*/
    if (!cmp)
    {
        return llist_push_back(list, data, sz1);
    }
    /*search until the node data is less than the data*/
    curnode = list->first;
    while (curnode)
    {
        if (cmp(curnode->data, data, userdata) < 0)
        {
            break;
        }
        curnode = curnode->next;
    }
    if (curnode)
    {
        return llist__insert(list, curnode, curnode->next, data, sz1);
    }
    /*default push back*/
    return llist_push_back(list, data, sz1);
}

void llist_update(struct llist_node_s* node, void* data, int sz1)
{
    if (data && sz1)
    {
        void* newdata = calloc(1, sz1);
        if (newdata)
        {
            memcpy(newdata, data, sz1);
            free(node->data);
            node->data = newdata;
        }
    }
}

struct llist_node_s* llist_find(
    llist_t* list,
    void* data,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata)
{
    list->listcmp = cmp;
    list->cmpdata = data;
    list->cmp_userdata = userdata;
    return llist_find_next(list, 0);
}

struct llist_node_s* llist_find_next(
    llist_t* list,
    struct llist_node_s* start)
{
    struct llist_node_s* node = (start ? start : list->first);
    if (!list->listcmp || !list->cmpdata || !node)
    {
        return 0;
    }
    while (node)
    {
        if (0 == list->listcmp(node->data, list->cmpdata, list->cmp_userdata))
        {
            return node;
        }
        node = node->next;
    }
    return 0;
}

struct llist_node_s* llist_rfind(
    llist_t* list,
    void* data,
    int (*cmp)(void* p1, void* p2, void* userdata),
    void* userdata)
{
    list->listcmp = cmp;
    list->cmpdata = data;
    list->cmp_userdata = userdata;
    return llist_find_prev(list, 0);
}

struct llist_node_s* llist_find_prev(
    llist_t* list,
    struct llist_node_s* start)
{
    struct llist_node_s* node = (start ? start : list->last);
    if (!list->listcmp || !list->cmpdata || !node)
    {
        return 0;
    }
    while (node)
    {
        if (0 == list->listcmp(node->data, list->cmpdata, list->cmp_userdata))
        {
            return node;
        }
        node = node->prev;
    }
    return 0;
}

