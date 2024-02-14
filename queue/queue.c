/*
File name: queue.c
Author: Seree Meo Rakwong
Date: 02-FEB-2024
Purpose: Implement a queue
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#ifdef __cplusplus
extern "C"
{
#endif
struct queue_node_s
{
    struct queue_node_s* next;
    void*   data;
};

struct queue_s
{
    struct queue_node_s* first;
    struct queue_node_s* last;
    int nitems;
};


queue_t* queue_new()
{
    queue_t* queue = (struct queue_s*)calloc(1, sizeof(struct queue_s));
    if (!queue)
    {
        return 0;
    }
    return queue;
}

void queue_del(queue_t* queue)
{
    if (queue)
    {
        queue_deque_all(queue);
        free(queue);
    }
}

void queue_deque_all(queue_t* queue)
{
    struct queue_node_s* first = queue->first;
    while (first)
    {
        queue_deque(queue);
        first = queue->first;
    }
    queue->first = queue->last = 0;
    queue->nitems = 0;
}

void queue_deque(queue_t* queue)
{
    queue_deque_keep_data(queue, 0); /*not kept data (also free data)*/
}

void queue_deque_keep_data(queue_t* queue, int keep)
{
    struct queue_node_s* first = queue->first;
    if (first)
    {
        /*going the next*/
        queue->first = queue->first->next;
        --queue->nitems;
        if (0 == queue->nitems)
        {
            queue->first = queue->last = 0;
        }
        /*free*/
        first->next = 0;
        if (!keep)
        {
            free(first->data);
            first->data = 0;
        }
        free(first);
        first = 0;
    }
}

struct queue_node_s* queue__new_node(void* data, int sz1)
{
    struct queue_node_s* newnode = (struct queue_node_s*)calloc(1, sizeof(struct queue_node_s));
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

void* queue_enque(queue_t* queue, void* data, int sz1)
{
    struct queue_node_s* newnode = queue__new_node(data, sz1);
    if (!newnode)
    {
        return 0;
    }
    if (!queue->first)
    {
        queue->first = queue->last = newnode;
    }
    else
    {
        queue->last->next = newnode;
        queue->last = newnode;
    }
    ++queue->nitems;
    return newnode->data;
}

struct queue_node_s* queue_front(queue_t* queue)
{
    return (queue->first);
}

void* queue_get_data(struct queue_node_s* node)
{
    return (node ? node->data : 0);
}

#ifdef __cplusplus
}
#endif
