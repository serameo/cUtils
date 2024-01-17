#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

int cmpname(void* p1, void* p2, void* userdata)
{
    char* n1 = (char*)p1;
    char* n2 = (char*)p2;
    return strcmp(n1, n2);
}

int main()
{
    llist_t* list = llist_new();
    char name[32];
    struct llist_node_s* node;
    
    strcpy(name, "meo");
    llist_push_back(list, name, strlen(name) + 1);

    strcpy(name, "matt");
    llist_push_back(list, name, strlen(name) + 1);

    strcpy(name, "malte");
    llist_push_front(list, name, strlen(name) + 1);

    strcpy(name, "knij");
    llist_push_front(list, name, strlen(name) + 1);
    
    node = llist_rfind(list, name, cmpname, 0);
    if (node)
    {
        printf("found %s\n", name);
    }
    else
    {
        printf("not found %s\n", name);
    }
    llist_del(list);
    return 0;
}
