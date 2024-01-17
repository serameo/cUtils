/*
File name: stack.h
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a stack
*/

#ifndef __STACK_H__
#define __STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

struct stack_item_s;
struct stack_s;
typedef struct stack_s stack_t;

stack_t* stack_new(int nitems);
void stack_del(stack_t* stack);
void* stack_top(stack_t* stack);
int stack_empty(stack_t* stack);
int stack_full(stack_t* stack);
int stack_push(stack_t* stack, void* data, int sz1);
void stack_pop(stack_t* stack);



#ifdef __cplusplus
}
#endif


#endif /* __STACK_H__*/
