#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstree.h"

int test_cmp(void* p1, void* p2, void* userdata)
{
    int i = (p1  ? *(int*)p1  : -1);
    int j = (p2 ? *(int*)p2 : -1);
    if (!p1 || !p2)
    {
        /*printf("[p1:%d][p2:%d]...\n",
            (p1 ? i : -1),
            (p2 ? j : -2));*/
        return 1;
    }
    /*printf("[p1:%d,p2:%d]\n", i, j);*/
    return (i-j);
}

int main()
{
    int i;
    int *pi = 0;
    struct bstree_item_s* item = 0;
    bstree_t* bst = bstree_new(8);
    i = 5;
    bstree_set(bst, 0, &i, sizeof(int));
    i = 2;
    bstree_set(bst, 1, &i, sizeof(int));
    i = 1;
    bstree_set(bst, 2, &i, sizeof(int));
    i = 7;
    bstree_set(bst, 3, &i, sizeof(int));
    i = 0;
    bstree_set(bst, 4, &i, sizeof(int));
    i = 3;
    bstree_set(bst, 5, &i, sizeof(int));
    i = 6;
    bstree_set(bst, 6, &i, sizeof(int));
    i = 4;
    bstree_set(bst, 7, &i, sizeof(int));
    i = 8;
    bstree_set(bst, 8, &i, sizeof(int)); /* do nothing*/
    i = 8;
    bstree_push_back(bst, &i, sizeof(int)); /* re-allocate the new memory*/
    i = 16;
    bstree_push_back(bst, &i, sizeof(int));
    
    printf("test finding...\n");
    i = 3;
    pi = (int*)bstree_find(bst, &i, test_cmp, 0);
    if (pi)
    {
        printf("found %d\n", *pi);
    }
    else
    {
        printf("not found %d\n", *pi);
    }
    
    bstree_del(bst);
    return 0;
}
