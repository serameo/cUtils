#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uxtree.h"

int test_cmp(int level, void* cmpdata, void* finddata, void* userdata)
{
    printf("%*s-->%s\n", level*2, " ", (cmpdata ? (char*)cmpdata : "/"));
    return -1;
}

int main()
{
    char path[BUFSIZ + 1];
    uxtree_t* tree = uxtree_new();
    struct uxtree_node_s* home = uxtree_insert_sort(tree, 0, "home", strlen("home") + 1, "home");
    struct uxtree_node_s* usr2 = uxtree_insert_sort(tree, home, "usr",  strlen("usr") + 1,  "usr");
    struct uxtree_node_s* meo  = uxtree_insert_sort(tree, usr2, "meo",  strlen("meo") + 1,  "meo");
    struct uxtree_node_s* dev  = uxtree_insert_sort(tree, home, "dev",  strlen("dev") + 1,  "dev");
    struct uxtree_node_s* bin  = uxtree_insert_sort(tree, 0, "bin",  strlen("bin") + 1,  "bin");
    struct uxtree_node_s* usr  = uxtree_insert_sort(tree, 0, "usr",  strlen("usr") + 1,  "usr");
    struct uxtree_node_s* lib  = uxtree_insert_sort(tree, usr, "lib",  strlen("lib") + 1,  "lib");
    struct uxtree_node_s* include  = uxtree_insert_sort(tree, usr, "include",  strlen("include") + 1,  "include");
    struct uxtree_node_s* uv       = uxtree_insert_sort(tree, include, "uv",  strlen("uv") + 1,  "uv");
    struct uxtree_node_s* uv_h     = uxtree_insert_sort(tree, uv, "uv.h",  strlen("uv.h") + 1,  "uv.h");
    struct uxtree_node_s* tree_h   = uxtree_insert_sort(tree, uv, "tree.h",  strlen("tree.h") + 1,  "tree.h");
    
    /*struct uxtree_node_s* node = uxtree_find_path(tree, "/lib");*/
    struct uxtree_node_s* node = uxtree_find_path(tree, "/usr/include/uv/ ");
    if (node)
    {
        uxtree_get_path(tree, node, path, BUFSIZ);
        printf("found: %s\n", path);
    }
    /*print all nodes*/
    node = uxtree_find(tree, (void*)"abc", test_cmp,  0);
    /*remove*/
    node = uxtree_find_path(tree, "/usr/include/uv/");
    uxtree_remove(tree, node);
    /*try to find it again*/
    node = uxtree_find_path(tree, "/usr/include/uv/");
    if (node)
    {
        printf("removed failed\n");
    }
    else
    {
        printf("removed successfully\n");
    }
    /*to review the remove by printing all nodes*/
    node = uxtree_find(tree, (void*)"abc", test_cmp,  0);
    
    uxtree_del(tree);
    return 0;
}
