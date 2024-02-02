/*
File name: xcnf.c
Author: Seree Meo Rakwong
Date: 11-JAN-2024
Purpose: Implement a simple configuration file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstree.h"
#include "xcnf.h"

#ifdef __cplusplus
extern "C"
{
#endif
/*
# this is a line comment
screen.width = 800
screen.height = 640
screen.title = "This is a first windows"

#maximum character in 1 line = 1024
*/

#define XCNF_MAX_LINE_LEN       1024
#define XCNF_COMMENT_1          '!'
#define XCNF_COMMENT_2          ';'
#define XCNF_COMMENT_3          '#'
#define XCNF_QOUTE              '"' /*to scope the value*/
#define XCNF_SECTION_BEGIN      '['
#define XCNF_SECTION_END        ']'
#define XCNF_ARRAY_BEGIN        '['
#define XCNF_ARRAY_END          ']'


struct xcnf_node_s;
struct xcnf_line_s;
struct xcnf_section_s;

#define XCNF_EMSG_EOS_QUOTE             "require double-quote at the end-of-string"
#define XCNF_EMSG_INSUFFICIENT_MEMORY   "Insufficient memory"

const char* xcnf__errmsg = "";

struct xcnf_s
{
    bstree_t*     configs;                  /*keep the sections*/
    struct xcnf_line_s*     fline;          /*keep the original file info*/
    struct xcnf_line_s*     lline;          /*keep the original file info*/
    struct xcnf_section_s*  fsection;
    struct xcnf_section_s*  lsection;
};

/*
e.g. 1 (no section found before the key and the value)
screen.width = 800
#section => null
#key => screen.width
#value => 800
#key MUST NOT HAVE spaces
#value could have spaces but its value will be under double-quote
#e.g. screen.title = "hello world"
*/
/*
e.g. 2 (there was a section before the key and the value)
[section screen]
screen.width = 800
#section => "section screen"
#key => screen.width
#value => 800
*/
struct xcnf_node_s
{
    char*   section;    /*never allocated*/
    char*   key;
    char*   value;
};

/*
# this is a line comment
screen.width = 800
screen.height = 640
screen.title = "This is a first windows"
*/
#define XCNF_TYPE_BLANK     0   /*space(s) line*/
#define XCNF_TYPE_COMMENT   1   /*begin with '#' or ';' or '!'*/
#define XCNF_TYPE_KEYVAL    2   /*separate between key and value with '='*/
#define XCNF_TYPE_SECTION   3   /*begin with '[' and end with ']'*/
struct xcnf_line_s
{
    struct xcnf_line_s*     next;
    int     lineno;
    char*   text;
    int     type;   /*XCNF_TYPE_xxx*/
    struct xcnf_section_s*  section;    /*could be nil if type is not XCNF_TYPE_SECTION*/
    struct xcnf_node_s*     node;       /*could be nil if type is not XCNF_TYPE_KEYVAL*/
};

struct xcnf_section_s
{
    struct xcnf_section_s*  next;
    char*   name;
    struct xcnf_node_s*     nodes;
};

char* xcnf__tolower(char* s)
{
    for (char *p = s; *p; p++)
    {
        *p = tolower(*p);
    }
    return s;
}
char* xcnf__toupper(char* s)
{
    for (char *p = s; *p; p++)
    {
        *p = toupper(*p);
    }
    return s;
}

int xcnf__compare(void* p1, void* p2, void* userdata)
{
    struct xcnf_node_s* n1 = (struct xcnf_node_s*)p1;
    struct xcnf_node_s* n2 = (struct xcnf_node_s*)p2;
    int rc = -1;/*not compared yet*/
    
    if (!n1->section && !n2->section)
    {
        return strcmp(n1->key, n2->key);
    }
    if (n1->section && n2->section)
    {
        rc = strcmp(n1->section, n2->section);
        if (0 == rc)
        {
            /*both sections are identical*/
            rc = strcmp(n1->key, n2->key);
        }
    }
    return rc;
}

xcnf_t* xcnf_new()
{
    xcnf_t* xcnf = (xcnf_t*)calloc(1, sizeof(struct xcnf_s));
    if (!xcnf)
    {
        xcnf__errmsg = XCNF_EMSG_INSUFFICIENT_MEMORY;
        return 0;
    }
    xcnf->configs = bstree_new(-1);
    return xcnf;
}

void xcnf__free_lines(xcnf_t* xcnf)
{
    struct xcnf_line_s* line = xcnf->fline;
    struct xcnf_line_s* next;
    while (line)
    {
        next = line->next;
        free(line->text);
        line->text = 0;
        if (line->section)
        {
            free(line->section->name);
            free(line->section);
            line->section = 0;
        }
        if (line->node)
        {
            free(line->node->key);
            free(line->node->value);
            free(line->node);
            line->node = 0;
        }
        line = next;
    }
    xcnf->fline = xcnf->lline = 0;
}

void xcnf_del(xcnf_t* xcnf)
{
    xcnf__errmsg = "";
    if (xcnf)
    {
        bstree_del(xcnf->configs);
        xcnf->configs = 0;

        /*free lines*/
        xcnf__free_lines(xcnf);
        free(xcnf);
    }
}

const char* xcnf_get_errmsg()
{
    return xcnf__errmsg;
}

int xcnf__parse(xcnf_t* xcnf, char* text, int lineno)
{
    int rc = 0;
    struct xcnf_node_s* node = 0;
    struct xcnf_section_s* section = 0;
    struct xcnf_line_s* line = 0;
    int type = XCNF_TYPE_BLANK;
    char* key = 0;
    char* value = 0;
    char* value1 = 0;
    char* value2 = 0;
    char* token;
    char* line_text = (char*)calloc(1, 1 + strlen(text)); /*backup the text*/

    strcpy(line_text, text);
    token = strtok(line_text, " \t\r\n");
    if (!token)
    {
        /*empty line*/
        type = XCNF_TYPE_BLANK;
        goto create_line;
    }

    if (XCNF_COMMENT_1 == token[0] ||
        XCNF_COMMENT_2 == token[0] ||
        XCNF_COMMENT_3 == token[0])
    {
        /*comment line*/
        type = XCNF_TYPE_COMMENT;
        goto create_line;
    }

    if (XCNF_SECTION_BEGIN == token[0] &&
        XCNF_SECTION_END   == token[strlen(token) - 1])
    {
        /*section*/
        type = XCNF_TYPE_SECTION;
        goto create_section;
    }

    strcpy(line_text, text);
    type = XCNF_TYPE_KEYVAL;
    token = strtok(line_text, "= ");
    if (token)
    {
        key = token;
        token = strtok(NULL, "= \r\n");
        if (token)
        {
            if (XCNF_QOUTE == token[0])
            {
                value1 = token;
                token = strtok(NULL, "\"");
                if (token)
                {
                    value2 = token;
                }
                else
                {
                    xcnf__errmsg = "require double-quote at the end-of-string";
                    value1 = 0;
                }
            }
            else
            {
                value = value1 = token;
            }
        }
    }
    if (!key || !value1)
    {
        free(line_text);
        return -1;  /*no key or value*/
    }
    
create_section:
    if (XCNF_TYPE_SECTION == type)
    {
        section = (struct xcnf_section_s*)calloc(1, sizeof(struct xcnf_section_s));
        section->name = (char*)calloc(1, strlen(token));
        strncpy(section->name, &token[1], strlen(token) - 2);
        if (xcnf->fsection)
        {
            xcnf->lsection->next = section;
            xcnf->lsection = section;
        }
        else
        {
            xcnf->fsection = xcnf->lsection = section;
        }
    }

create_node:
    if (XCNF_TYPE_KEYVAL == type)
    {
        node = (struct xcnf_node_s*)calloc(1, sizeof(struct xcnf_node_s));
        node->section = (xcnf->lsection ? xcnf->lsection->name : 0);
        node->key = (char*)calloc(1, 1 + strlen(key));
        strcpy(node->key, key);
        if (value1 && value2)
        {
            node->value = (char*)calloc(1, 2 + strlen(value1) + strlen(value2));
            strcpy(node->value, &value1[1]);
            strcat(node->value, " ");
            strcat(node->value, value2);
        }
        else if (value1)
        {
            node->value = (char*)calloc(1, 1 + strlen(value1));
            strcpy(node->value, value1);
        }
        /*insert to tree*/
        bstree_push_back(xcnf->configs, node, sizeof(struct xcnf_node_s));
    }

create_line:
    line = (struct xcnf_line_s*)calloc(1, sizeof(struct xcnf_line_s));
    line->lineno = lineno;
    line->text = line_text;
    strcpy(line->text, text);
    line->type = type;
    line->section = section;
    line->node = node;

    if (xcnf->fline)
    {
        xcnf->lline->next = line;
        xcnf->lline = line;
    }
    else
    {
        xcnf->fline = xcnf->lline = line;
    }
    
    return rc;
}

int xcnf_load(xcnf_t* xcnf, FILE* fp)
{
    int rc = 0;
    int lineno = 1;
    char line[XCNF_MAX_LINE_LEN + 1];

    xcnf__errmsg = "";
    while (!feof(fp))
    {
        memset(line, 0, sizeof(line));
        if (!fgets(line, XCNF_MAX_LINE_LEN, fp))
        {
            break;
        }
        rc = xcnf__parse(xcnf, line, lineno);
        if (0 != rc)
        {
            fprintf(stderr, "LINE %d: incorrect format (e.g. key=\"value\")\n", lineno);
            break;
        }
        ++lineno;
    }
    return rc;
}

char* xcnf_get(xcnf_t* xcnf, char* section, char* key, char* def)
{
    struct bstree_item_s* item = 0;
    struct xcnf_node_s* founddata = 0;
    struct xcnf_node_s finddata;
    
    xcnf__errmsg = "";

    finddata.section = section;
    finddata.key = key;
    item = bstree_find(xcnf->configs, &finddata, xcnf__compare, xcnf);

    founddata = (struct xcnf_node_s*)(item ? (char*)bstree_get_data(item) : 0);
    return (founddata ? founddata->value : def);
}

int xcnf_get_int(xcnf_t* xcnf, char* section, char* key, int def)
{
    char* ret = xcnf_get(xcnf, section, key, "0");
    return (ret ? atoi(ret) : def);
}

double xcnf_get_float(xcnf_t* xcnf, char* section, char* key, double def)
{
    char* ret = xcnf_get(xcnf, section, key, "0.0");
    return (ret ? atof(ret) : def);
}


#ifdef __cplusplus
}
#endif
