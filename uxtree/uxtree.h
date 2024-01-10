/*
File name: uxtree.h
Author: Seree Meo Rakwong
Date: 08-JAN-2024
*/

#ifndef __uxtree_H__
#define __uxtree_H__

#ifdef __cplusplus
extern "C" {
#endif

struct uxtree_node_s;  /*to be referenced*/
struct uxtree_s;
typedef struct uxtree_s uxtree_t;

uxtree_t* uxtree_new();
void      uxtree_del(uxtree_t* tree);

struct  uxtree_node_s* uxtree_insert(uxtree_t* tree, struct uxtree_node_s* parent, const void* data, int sz1, const char* nm1);
struct  uxtree_node_s* uxtree_insert_sort(uxtree_t* tree, struct uxtree_node_s* parent, const void* data, int sz1, const char* nm1);
void    uxtree_remove(uxtree_t* tree, struct uxtree_node_s* delnode);
void    uxtree_remove_all(uxtree_t* tree);
struct  uxtree_node_s* uxtree_find(
            uxtree_t* tree,
            void* finddata,
            int (*cmp)(int level, void* cmpdata, void* finddata, void* userdata),
            void* userdata);
struct  uxtree_node_s* uxtree_find_path(uxtree_t* tree, const char* path);
int     uxtree_get_path(uxtree_t* tree, struct uxtree_node_s* node, char* outpath, int outlen);
/*struct uxtree_node_s properties*/
void    uxtree_update(struct uxtree_node_s* node, void* data, int sz1);
void    uxtree_rename(struct uxtree_node_s* node, const char* newname);
int     uxtree_children(struct uxtree_node_s* node);
const char* uxtree_get_name(struct uxtree_node_s* node);
void*       uxtree_get_data(struct uxtree_node_s* node);
int         uxtree_get_level(struct uxtree_node_s* node);
struct uxtree_node_s* uxtree_get_parent(struct uxtree_node_s* node);
struct uxtree_node_s* uxtree_get_firstchild(struct uxtree_node_s* node);
struct uxtree_node_s* uxtree_get_lastchild(struct uxtree_node_s* node);
struct uxtree_node_s* uxtree_get_prev(struct uxtree_node_s* node);
struct uxtree_node_s* uxtree_get_next(struct uxtree_node_s* node);

#ifdef __cplusplus
}
#endif

#endif /* __uxtree_H__ */

