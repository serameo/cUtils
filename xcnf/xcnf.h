/*
File name: xcnf.h
Author: Seree Meo Rakwong
Date: 11-JAN-2024
Purpose: Implement a simple configuration file
*/

#ifndef __XCNF_H__
#define __XCNF_H__


#include <stdio.h> /* FILE* */
/*
example: abc.cnf
# this is a line comment
screen.width = 800
screen.height = 640
screen.title = "This is a first windows"

#maximum character in 1 line = 1024
*/

#ifdef __cplusplus
extern "C" {
#endif

struct xcnf_s;
typedef struct xcnf_s xcnf_t;

xcnf_t* xcnf_new();
void    xcnf_del(xcnf_t* xcnf);
int     xcnf_load(xcnf_t* xcnf, FILE* fp);
char*   xcnf_get(xcnf_t* xcnf, char* section, char* key, char* def);
int     xcnf_get_int(xcnf_t* xcnf, char* section, char* key, int def);
double  xcnf_get_float(xcnf_t* xcnf, char* section, char* key, double def);
const char* xcnf_get_errmsg();

#ifdef __cplusplus
}
#endif


#endif /* __XCNF_H__*/
