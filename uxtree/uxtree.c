/*
File name: uxtree.c
Author: Seree Meo Rakwong
Date: 08-JAN-2024
Purpose: Implement a tree like unix tree system
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uxtree.h"

struct uxt_node_s
{
    struct uxt_node_s*   parent;
    struct uxt_node_s*   fchild; /*first child*/
    struct uxt_node_s*   lchild; /*last child*/
    struct uxt_node_s*   prev;   /*previous sibling*/
    struct uxt_node_s*   next;   /*next sibling*/
    void*   data;
    char*   name;
    int     nchild;     /*number of children*/
    int     nlevel;     /*tree level*/
};

struct uxt_s
{
    struct uxt_node_s*   root;   /*always be '/'*/
};

struct uxt_node_s* uxt__new_node(const void* data, int sz1, const char* nm1)
{
    struct uxt_node_s* node = (struct uxt_node_s*)calloc(1, sizeof(struct uxt_node_s));
    if (!node)
    {
        return 0;
    }
    if (sz1 > 0 && data)
    {
        node->data = calloc(1, sz1);
        memcpy(node->data, data, sz1);
    }
    if (nm1 && strlen(nm1) > 0)
    {
        node->name = (char*)calloc(1, 1 + strlen(nm1));
        strcpy(node->name, nm1);
    }
    return node;
};

uxt_t* uxt_new()
{
    uxt_t* tree = (uxt_t*)calloc(1, sizeof(struct uxt_s));
    if (!tree)
    {
        return 0;
    }
    tree->root = uxt__new_node(0, 0, "/");
    if (!tree->root)
    {
        free(tree);
        return 0;
    }
    return tree;
}

void uxt_del(uxt_t* tree)
{
    if (tree)
    {
        uxt_remove_all(tree);
        free(tree->root);
        free(tree);
    }
}

struct uxt_node_s* uxt__find_child(uxt_t* tree, struct uxt_node_s* startnode, const char* nm1)
{
    struct uxt_node_s* node = startnode;
    while (node)
    {
        if (0 == strcmp(nm1, node->name))
        {
            return node;
        }
        node = node->next;
    }
    return 0;
}

struct uxt_node_s* uxt_insert(uxt_t* tree, struct uxt_node_s* parent, const void* data, int sz1, const char* nm1)
{
    struct uxt_node_s* newnode = 0;
    struct uxt_node_s* child = 0;
    if (!parent)
    {
        parent = tree->root;
    }
    /*check all children MUST NOT BE THE SAME NAME*/
    child = uxt__find_child(tree, parent->fchild, nm1);
    if (child)
    {
        /*always not allowed duplicated name*/
        return 0;
    }
    newnode = uxt__new_node(data, sz1, nm1);
    if (!newnode)
    {
        return 0;
    }
    /*link the new child*/
    if (!parent->fchild)
    {
        parent->fchild = parent->lchild = newnode;
    }
    else
    {
        newnode->prev = parent->lchild;
        parent->lchild->next = newnode;
        parent->lchild = newnode;
    }
    newnode->parent = parent;
    newnode->nlevel = parent->nlevel + 1;
    /*add the child counter*/
    ++parent->nchild;

    return newnode;
}

struct uxt_node_s* uxt_insert_sort(uxt_t* tree, struct uxt_node_s* parent, const void* data, int sz1, const char* nm1)
{
    struct uxt_node_s* newnode = 0;
    struct uxt_node_s* child = 0;
    struct uxt_node_s* prev = 0;
    struct uxt_node_s* curnode = 0;
    if (!parent)
    {
        parent = tree->root;
    }
    /*check all children MUST NOT BE THE SAME NAME*/
    child = uxt__find_child(tree, parent->fchild, nm1);
    if (child)
    {
        /*always not allowed duplicated name*/
        return 0;
    }
    newnode = uxt__new_node(data, sz1, nm1);
    if (!newnode)
    {
        return 0;
    }
    /*find the less node*/
    curnode = parent->fchild;
    while (curnode)
    {
        if (0 < strcmp(curnode->name, nm1))
        {
            break;
        }
        prev = curnode;
        curnode = curnode->next;
    }
    /*link the new child*/
    if (!parent->fchild)
    {
        /*no child*/
        parent->fchild = parent->lchild = newnode;
    }
    else
    {
        if (prev)
        {
            /*insert previous*/
            newnode->prev = prev;
            if (prev)
            {
                newnode->next = prev->next;
                prev->next = newnode;
                if (prev == parent->lchild)
                {
                    parent->lchild = newnode;
                }
            }
        }
        else
        {
            newnode->next = parent->fchild;
            parent->fchild->prev = newnode;
            parent->fchild = newnode;
        }
    }
    newnode->parent = parent;
    newnode->nlevel = parent->nlevel + 1;
    /*add the child counter*/
    ++parent->nchild;

    return newnode;
}


void uxt__remove_node(struct uxt_node_s* parent, struct uxt_node_s* delnode)
{
    struct uxt_node_s* prev_lchild = 0;
    struct uxt_node_s* prev   = 0;
    struct uxt_node_s* next   = 0;

    if (!parent || !delnode)
    {
        return;
    }
    prev    = delnode->prev;
    next    = delnode->next;
    if (prev)
    {
        prev->next = next;
    }
    if (next)
    {
        next->prev = prev;
    }
    if (delnode == parent->lchild)
    {
        parent->lchild = prev;
        if (parent->lchild)
        {
            parent->lchild->next = 0;
        }
    }
    if (delnode == parent->fchild)
    {
        parent->fchild = next;
        if (parent->fchild)
        {
            parent->fchild->prev = 0;
        }
    }
    /*decrease the child counter*/
    --parent->nchild;
    /*no more child*/
    free(delnode->data);
    free(delnode);
}

void uxt__remove(uxt_t* tree, struct uxt_node_s* delnode)
{
    struct uxt_node_s* parent = 0;
    struct uxt_node_s* prev_lchild   = 0;
    struct uxt_node_s* lchild = 0;

    if (!delnode)
    {
        return;
    }
    parent  = delnode->parent;
    /* can delete it if it has no more child*/
    if (!delnode->lchild)
    {
        /*update parent info*/
        uxt__remove_node(parent, delnode);
        return;
    }
    /*delete the last child first*/
    prev_lchild   = delnode->lchild->prev;
    uxt__remove(tree, delnode->lchild);

    /* delete the previous last child */
    uxt__remove(tree, prev_lchild);
    
    /* the last */
    uxt__remove_node(parent, delnode);
}

void uxt_remove(uxt_t* tree, struct uxt_node_s* delnode)
{
    struct uxt_node_s* parent = (delnode ? delnode->parent : 0);
    uxt__remove(tree, delnode);
    /*uxt__remove_node(parent, delnode);*/
}

void uxt_remove_all(uxt_t* tree)
{
    struct uxt_node_s* delnode = tree->root->lchild;
    struct uxt_node_s* prev = 0;
    
    while (delnode)
    {
        prev = delnode->prev;
        uxt_remove(tree, delnode);
        delnode = prev;
    }
}

struct uxt_node_s* uxt__find(uxt_t* tree,
    struct uxt_node_s* startnode,
    void* finddata,
    int (*cmp)(int level, void* cmpdata, void* finddata, void* userdata),
    void* userdata)
{
    struct uxt_node_s* foundnode = 0;
    if (!startnode || !cmp || !finddata)
    {
        return 0;
    }
    if (0 == cmp(startnode->nlevel, startnode->data, finddata, userdata))
    {
        return startnode;
    }
    foundnode = uxt__find(tree, startnode->fchild, finddata, cmp, userdata);
    if (0 != foundnode)
    {
        return foundnode;
    }
    foundnode = uxt__find(tree, startnode->next, finddata, cmp, userdata);
    return foundnode;
}

struct uxt_node_s* uxt_find(uxt_t* tree, 
    void* finddata,
    int (*cmp)(int level, void* cmpdata, void* finddata, void* userdata),
    void* userdata)
{
    return uxt__find(tree, tree->root->fchild, finddata, cmp, userdata);
}

struct uxt_node_s* uxt__find_path(uxt_t* tree, struct uxt_node_s* startnode, const char* path)
{
    struct uxt_node_s* foundnode = 0;
    struct uxt_node_s* curnode = startnode;
    char psz[BUFSIZ + 1];
    char* token;
    char* pos;
    char subpath[BUFSIZ + 1];
    
    if (!curnode)
    {
        return 0;
    }
    strcpy(psz, path);
    token = strtok(psz, "/");
    if (!token)
    {
        return 0;
    }
    /*find the child node*/
    if (0 == strcmp(token, curnode->name))
    {
        if (0 == curnode->fchild)
        {
            /*last node*/
            token = strtok(NULL, "/ ");
            if (0 == token)
            {
                return curnode;
            }
        }
        else
        {
            pos = strstr(path, token);
            strcpy(subpath, &path[((long)pos - (long)path) + strlen(token)]);
            token = strtok(NULL, "/ ");
            if (0 == token)
            {
                return curnode;
            }
            return uxt__find_path(tree, curnode->fchild, subpath);
        }
    }
    else
    {
        /*find the sibling nodes*/
        return uxt__find_path(tree, curnode->next, path);
    }
    return 0;
}

struct uxt_node_s* uxt_find_path(uxt_t* tree, const char* path)
{
    return uxt__find_path(tree, tree->root->fchild, path);
}

int uxt_get_path(uxt_t* tree, struct uxt_node_s* node, char* outpath, int outlen)
{
    char subpath[BUFSIZ + 1];
    char parpath[BUFSIZ + 1];
    int len = 0;
    struct uxt_node_s* parent = (node ? node->parent : 0);
    if (!node)
    {
        return 0;
    }
    strcpy(subpath, "");
    while (parent && parent != tree->root)
    {
        strcpy(parpath, "/");
        strcat(parpath, parent->name);  /*new parent path*/
        strcat(parpath, subpath);       /*restore the subpath*/
        strcpy(subpath, parpath);
        parent = parent->parent;
    }
    strcat(parpath, "/");
    strcat(parpath, node->name);
    len = strlen(parpath);
    if (outlen < 0)
    {
        return len;
    }
    if (outpath)
    {
        memset(outpath, 0, outlen);
        if (outlen > len)
        {
            outlen = len;
        }
        memcpy(outpath, parpath, outlen);
    }
    return len;
}

/*struct uxt_node_s properties*/
void uxt_update(struct uxt_node_s* node, void* data, int sz1)
{
    if (sz1 > 0 && data)
    {
        void* newdata = calloc(1, sz1);
        if (newdata)
        {
            memcpy(newdata, data, sz1);
            free(node->data);
            node->data = newdata;
        }
    }
}

void uxt_rename(struct uxt_node_s* node, const char* name)
{
    if (name && strlen(name) > 0)
    {
        char* newname = (char*)calloc(1, 1 + strlen(name));
        if (newname)
        {
            strcpy(newname, name);
            free(node->name);
            node->name = newname;
        }
    }
}

int  uxt_children(struct uxt_node_s* node)
{
    return (node ? node->nchild : -1);
}

const char* uxt_get_name(struct uxt_node_s* node)
{
    return (node ? (const char*)node->name : 0);
}

void* uxt_get_data(struct uxt_node_s* node)
{
    return (node ? node->data : 0);
}

int uxt_get_level(struct uxt_node_s* node)
{
    return (node ? node->nlevel : -1);
}


struct uxt_node_s* uxt_get_parent(struct uxt_node_s* node)
{
    return (node ? node->parent : 0);
}

struct uxt_node_s* uxt_get_firstchild(struct uxt_node_s* node)
{
    return (node ? node->fchild : 0);
}

struct uxt_node_s* uxt_get_lastchild(struct uxt_node_s* node)
{
    return (node ? node->lchild : 0);
}

struct uxt_node_s* uxt_get_prev(struct uxt_node_s* node)
{
    return (node ? node->prev : 0);
}
struct uxt_node_s* uxt_get_next(struct uxt_node_s* node)
{
    return (node ? node->next : 0);
}
