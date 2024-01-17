#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int main()
{
    queue_t* queue = queue_new();
    char name[32];
    
    strcpy(name, "meo");
    queue_enque(queue, name, strlen(name) + 1);

    strcpy(name, "matt");
    queue_enque(queue, name, strlen(name) + 1);

    strcpy(name, "malte");
    queue_enque(queue, name, strlen(name) + 1);

    strcpy(name, "knij");
    queue_enque(queue, name, strlen(name) + 1);
    
    printf("front: %s\n", (char*)queue_front(queue));
    queue_del(queue);
    
    return 0;
}
