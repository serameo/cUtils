/*
File name: queue.h
Author: Seree Meo Rakwong
Date: 02-FEB-2024
Purpose: Implement a queue
*/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

struct queue_node_s;
struct queue_s;
typedef struct queue_s queue_t;

queue_t* queue_new();
void queue_del(queue_t* queue);
void queue_deque_all(queue_t* queue);
void queue_deque(queue_t* queue);
void queue_deque_keep_data(queue_t* queue, int keep);
void* queue_enque(queue_t* queue, void* data, int sz1);
struct queue_node_s* queue_front(queue_t* queue);
void* queue_get_data(struct queue_node_s* node);


#ifdef __cplusplus
}
#endif

#endif /*__QUEUE_H__*/