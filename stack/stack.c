/*
File name: stack.c
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a stack
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"


struct stack_item_s
{
    void*   data;
};

struct stack_s
{
    struct stack_item_s*    items;
    int nitems;
    int idx;
};


stack_t* stack_new(int nitems)
{
    stack_t* stack = (struct stack_s*)calloc(1, sizeof(struct stack_s));
    if (!stack)
    {
        return 0;
    }
    if (nitems < 1)
    {
        nitems = 64;
    }
    stack->items = (struct stack_item_s*)calloc(nitems, sizeof(struct stack_item_s));
    if (!stack->items)
    {
        free(stack);
        return 0;
    }
    stack->nitems = nitems;
    stack->idx    = -1;
    
    return stack;
}

void stack__free_item(stack_t* stack, int idx)
{
    free(stack->items[idx].data);
    stack->items[idx].data = 0;
}

void stack_del(stack_t* stack)
{
    if (stack)
    {
        int i = 0;
        for (i = 0; i < stack->idx; ++i)
        {
            stack__free_item(stack, i);
        }
        free(stack->items);
        stack->items = 0;
        stack->nitems = -1;
        stack->idx = -1;
        free(stack);
    }
}

void* stack_top(stack_t* stack)
{
    if (stack->idx >= 0)
    {
        return &stack->items[stack->idx].data;
    }
    return 0;
}

int stack_empty(stack_t* stack)
{
    return (stack->idx < 0 ? 1 : 0);
}

int stack_full(stack_t* stack)
{
    return (stack->idx + 1 >= stack->nitems ? 1 : 0);
}

void* stack__new_data(void* data, int sz1)
{
    void* newdata = 0;
    if (data && sz1 > 0)
    {
        newdata = calloc(1, sz1);
        if (newdata)
        {
            memcpy(newdata, data, sz1);
        }
    }
    return newdata;
}

int stack_push(stack_t* stack, void* data, int sz1)
{
    void* newdata = 0;
    if (stack_full(stack))
    {
        return 1;
    }
    newdata = stack__new_data(data, sz1);
    if (!newdata)
    {
        return -1;
    }
    ++stack->idx;
    stack->items[stack->idx].data = newdata;
    return 0;
}

void stack_pop(stack_t* stack)
{
    if (stack_empty(stack))
    {
        return;
    }
    stack__free_item(stack, stack->idx);
    --stack->idx;
}

