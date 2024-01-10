/*
File name: uxtree.h
Author: Seree Meo Rakwong
Date: 08-JAN-2024
*/

#ifndef __uxt_H__
#define __uxt_H__

#ifdef __cplusplus
extern "C" {
#endif

struct uxt_node_s;  /*to be referenced*/
struct uxt_s;
typedef struct uxt_s uxt_t;

uxt_t* uxt_new();
void      uxt_del(uxt_t* tree);

struct  uxt_node_s* uxt_insert(uxt_t* tree, struct uxt_node_s* parent, const void* data, int sz1, const char* nm1);
struct  uxt_node_s* uxt_insert_sort(uxt_t* tree, struct uxt_node_s* parent, const void* data, int sz1, const char* nm1);
void    uxt_remove(uxt_t* tree, struct uxt_node_s* delnode);
void    uxt_remove_all(uxt_t* tree);
struct  uxt_node_s* uxt_find(
            uxt_t* tree,
            void* finddata,
            int (*cmp)(int level, void* cmpdata, void* finddata, void* userdata),
            void* userdata);
struct  uxt_node_s* uxt_find_path(uxt_t* tree, const char* path);
int     uxt_get_path(uxt_t* tree, struct uxt_node_s* node, char* outpath, int outlen);
/*struct uxt_node_s properties*/
void    uxt_update(struct uxt_node_s* node, void* data, int sz1);
void    uxt_rename(struct uxt_node_s* node, const char* newname);
int     uxt_children(struct uxt_node_s* node);
const char* uxt_get_name(struct uxt_node_s* node);
void*       uxt_get_data(struct uxt_node_s* node);
int         uxt_get_level(struct uxt_node_s* node);
struct uxt_node_s* uxt_get_parent(struct uxt_node_s* node);
struct uxt_node_s* uxt_get_firstchild(struct uxt_node_s* node);
struct uxt_node_s* uxt_get_lastchild(struct uxt_node_s* node);
struct uxt_node_s* uxt_get_prev(struct uxt_node_s* node);
struct uxt_node_s* uxt_get_next(struct uxt_node_s* node);

#ifdef __cplusplus
}
#endif

#endif /* __uxt_H__ */

