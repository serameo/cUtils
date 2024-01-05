/*
File name: rbtree.h
Author: Seree Meo Rakwong
Date: 05-JAN-2024
*/
#ifndef __RBTREE_H__
#define __RBTREE_H__
#include "tree.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rbtree_node_s;  /*to be referenced*/
struct rbtree_s;
typedef struct rbtree_s rbtree_t;
typedef int     (*rbtree_cmp_f)(void* p1, void* p2);
typedef void*   (*rbtree_dup_f)(void* p1, int sz1);
typedef void    (*rbtree_rel_f)(void* p1);

#define RBTREE_CMP_INT_DECLARE(cmp_fn1, data_type, int_member1)\
static int cmp_fn1(void* p1, void* p2)\
{\
    data_type *q1, *q2;\
    q1 = (data_type*)p1;\
    q2 = (data_type*)p2;\
    return (q1->int_member1 - q2->int_member1);\
}

#define RBTREE_CMP_STR_DECLARE(cmp_fn1, data_type, str_member1)\
static int cmp_fn1(void* p1, void* p2)\
{\
    data_type *q1, *q2;\
    q1 = (data_type*)p1;\
    q2 = (data_type*)p2;\
    return strcmp(q1->str_member1, q2->str_member1);\
}

#define RBTREE_CMP_MEM_DECLARE(cmp_fn1, data_type, member1, len1)\
static int cmp_fn1(void* p1, void* p2)\
{\
    data_type *q1, *q2;\
    q1 = (data_type*)p1;\
    q2 = (data_type*)p2;\
    return memcmp(q1->member1, q2->member1, len1);\
}
/*simple functions to be initialized called by rbtree_new()*/
int     rbtree_cmp_int(void* p1, void* p2);
int     rbtree_cmp_string(void* p1, void* p2);
void*   rbtree_dup(void* p1, int sz1);
void    rbtree_rel(void* p1);

/*rbtree_get_xxx()*/
struct rbtree_node_s* rbtree_get_root(rbtree_t* rbtree);
struct rbtree_node_s* rbtree_get_left(rbtree_t* rbtree, struct rbtree_node_s* n);
struct rbtree_node_s* rbtree_get_right(rbtree_t* rbtree, struct rbtree_node_s* n);
struct rbtree_node_s* rbtree_get_next(rbtree_t* rbtree, struct rbtree_node_s* n);
struct rbtree_node_s* rbtree_get_prev(rbtree_t* rbtree, struct rbtree_node_s* n);
struct rbtree_node_s* rbtree_get_min(rbtree_t* rbtree);
struct rbtree_node_s* rbtree_get_max(rbtree_t* rbtree);
void*  rbtree_get_data(struct rbtree_node_s* n);
/*rbtree_xxx()*/
rbtree_t* rbtree_new(rbtree_cmp_f cmp, rbtree_dup_f dup, rbtree_rel_f rel);
struct rbtree_node_s* rbtree_insert(rbtree_t* rbtree, void* p1, int sz1);
struct rbtree_node_s* rbtree_insert_string(rbtree_t* rbtree, char* str1, int len1);
struct rbtree_node_s* rbtree_insert_int(rbtree_t* rbtree, int val1);
struct rbtree_node_s* rbtree_find(rbtree_t* rbtree, void* p1);
struct rbtree_node_s* rbtree_find_int(rbtree_t* rbtree, int p1);
struct rbtree_node_s* rbtree_find_string(rbtree_t* rbtree, char* p1);
struct rbtree_node_s* rbtree_nfind(rbtree_t* rbtree, void* p1);
struct rbtree_node_s* rbtree_nfind_int(rbtree_t* rbtree, int p1);
struct rbtree_node_s* rbtree_nfind_string(rbtree_t* rbtree, char* p1);
void rbtree_remove(rbtree_t* rbtree, void* p1);
void rbtree_remove_int(rbtree_t* rbtree, int val1);
void rbtree_remove_string(rbtree_t* rbtree, char* str1);
void rbtree_remove_all(rbtree_t* rbtree);
void rbtree_foreach(rbtree_t* rbtree, int (*callback)(void* p1, void* userdata), void* userdata);
/*rbtree_update()*/
void rbtree_update(rbtree_t* rbtree, struct rbtree_node_s* n1, void* p1, int sz1);
void rbtree_update_int(rbtree_t* rbtree, struct rbtree_node_s* n1, int p1);
void rbtree_update_string(rbtree_t* rbtree, struct rbtree_node_s* n1, char* p1);

/*utilities*/
#define RBTREE_LEAF_NODE        0       /*always send data = nil*/
#define RBTREE_NULL_NODE        1
#define RBTREE_NODE             2
#define RBTREE_LEFT_BEGIN       -10     /*always send data = nil*/
#define RBTREE_LEFT_END         -11     /*always send data = nil*/
#define RBTREE_RIGHT_BEGIN      -20     /*always send data = nil*/
#define RBTREE_RIGHT_END        -21     /*always send data = nil*/

void rbtree_print(rbtree_t* rbtree, struct rbtree_node_s *n1, void (*print_f)(void* p1, int flag, void* userdata), void* userdata);


#ifdef __cplusplus
}
#endif

#endif /* __RBTREE_H__*/
