/*
File name: mapdb.h
Author: Seree Meo Rakwong
Date: 17-JAN-2024

NOTE:
  VMS  - LINK proc,bos_lib:sqlite3.opt/opt
  UNIX - gcc proc.c -lsqlite3
 */
#ifndef __MAPDB_H__
#define __MAPDB_H__

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mapdb_s;
typedef struct mapdb_s mapdb_t;

mapdb_t* mapdb_new();
void   mapdb_del(mapdb_t*);

typedef long (*fn_mapdb_populate_callback)(char* key, void* valp, const void* userdata);
/*
name - the unique identifier to be created a table in sqlite3 db
*/
int mapdb_init(mapdb_t* map, char* name);
int mapdb_release(mapdb_t* map, char* name);
int mapdb_insert(mapdb_t* map, char* name, char* key, const void* valp, unsigned long size);
int mapdb_erase(mapdb_t* map, char* name, char* key);
int mapdb_clear(mapdb_t* map, char* name);
void* mapdb_find(mapdb_t* map, char* name, char* key);
long mapdb_count(mapdb_t* map, char* name);
long mapdb_populate(  mapdb_t* map, 
                    char* name, 
                    fn_mapdb_populate_callback callback,
                    const void* userdata);
long mapdb_reverse_populate(  mapdb_t* map, 
                            char* name, 
                            fn_mapdb_populate_callback callback,
                            const void* userdata);
int mapdb_exist(mapdb_t* map, char* name);


#define SYS_MAPDB_NAME    "S00"   /* Application CANNOT use this name */




#ifdef __cplusplus
}
#endif

#endif /* __MAPDB_H__ */
