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
    uxt_t* tree = uxt_new();
    struct uxt_node_s* home = uxt_insert_sort(tree, 0, "home", strlen("home") + 1, "home");
    struct uxt_node_s* usr2 = uxt_insert_sort(tree, home, "usr",  strlen("usr") + 1,  "usr");
    struct uxt_node_s* meo  = uxt_insert_sort(tree, usr2, "meo",  strlen("meo") + 1,  "meo");
    struct uxt_node_s* dev  = uxt_insert_sort(tree, home, "dev",  strlen("dev") + 1,  "dev");
    struct uxt_node_s* bin  = uxt_insert_sort(tree, 0, "bin",  strlen("bin") + 1,  "bin");
    struct uxt_node_s* usr  = uxt_insert_sort(tree, 0, "usr",  strlen("usr") + 1,  "usr");
    struct uxt_node_s* lib  = uxt_insert_sort(tree, usr, "lib",  strlen("lib") + 1,  "lib");
    struct uxt_node_s* include  = uxt_insert_sort(tree, usr, "include",  strlen("include") + 1,  "include");
    struct uxt_node_s* uv       = uxt_insert_sort(tree, include, "uv",  strlen("uv") + 1,  "uv");
    struct uxt_node_s* uv_h     = uxt_insert_sort(tree, uv, "uv.h",  strlen("uv.h") + 1,  "uv.h");
    struct uxt_node_s* tree_h   = uxt_insert_sort(tree, uv, "tree.h",  strlen("tree.h") + 1,  "tree.h");
    
    /*struct uxt_node_s* node = uxt_find_path(tree, "/lib");*/
    struct uxt_node_s* node = uxt_find_path(tree, "/usr/include/uv/ ");
    if (node)
    {
        uxt_get_path(tree, node, path, BUFSIZ);
        printf("found: %s\n", path);
    }
    /*print all nodes*/
    node = uxt_find(tree, (void*)"abc", test_cmp,  0);
    /*remove*/
    node = uxt_find_path(tree, "/usr/include/uv/");
    uxt_remove(tree, node);
    /*try to find it again*/
    node = uxt_find_path(tree, "/usr/include/uv/");
    if (node)
    {
        printf("removed failed\n");
    }
    else
    {
        printf("removed successfully\n");
    }
    /*to review the remove by printing all nodes*/
    node = uxt_find(tree, (void*)"abc", test_cmp,  0);
    
    uxt_del(tree);
    return 0;
}
