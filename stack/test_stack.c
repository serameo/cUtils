#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

int main()
{
    stack_t* stack = stack_new(4);
    char name[32];
    strcpy(name, "meo");
    stack_push(stack, name, strlen(name) + 1);

    strcpy(name, "matt");
    stack_push(stack, name, strlen(name) + 1);

    strcpy(name, "malte");
    stack_push(stack, name, strlen(name) + 1);

    strcpy(name, "knij");
    stack_push(stack, name, strlen(name) + 1);

    strcpy(name, "unknown");
    stack_push(stack, name, strlen(name) + 1);
    
    stack_del(stack);

    return 0;
}
