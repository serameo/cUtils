/*
File name: mapdb.c
Author: Seree Meo Rakwong
Date: 17-JAN-2024

NOTE:
  VMS  - LINK proc,bos_lib:sqlite3.opt/opt
  UNIX - gcc proc.c -lsqlite3
 */
#include <stdlib.h>
#include <string.h>
#include "mapdb.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mapdb_s
{
    sqlite3* memdb;
};

long _mapdb_populate( mapdb_t* map, 
                    char* name, 
                    fn_mapdb_populate_callback callback, 
                    const void* vp,
                    int reverse);
long _mapdb_clear_callback(char* key, void* valp, const void* userdata);
int _mapdb_free(mapdb_t* map);
int _mapdb_free_name(mapdb_t* map, char* name);
void _mapdb_node_release(void* node);
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
#define USING_MEMDB                         ":memory:"
#define SELECT_FROM_NAME_ORDER_MAPKEY_QS    "SELECT MAPVAL, MAPKEY FROM %q ORDER BY MAPKEY %s;"
#define SELECT_FROM_NAME_ORDER_MAPKEY_DESC_Q    "SELECT MAPVAL, MAPKEY FROM %q ORDER BY MAPKEY DESC;"
#define SELECT_FROM_NAME_ORDER_MAPKEY_ASC_Q     "SELECT MAPVAL, MAPKEY FROM %q ORDER BY MAPKEY ASC;"
#define CREATE_TABLE_SYS_MAPDB_NAME_Q         "CREATE TABLE %q("\
                                            " MAPNAME VARCHAR(256) PRIMARY KEY NOT NULL"\
                                            ");"
#define CREATE_TABLE_NAME_Q                 "CREATE TABLE %q("\
                                            " MAPKEY VARCHAR(256) PRIMARY KEY NOT NULL,"\
                                            " MAPVAL INTEGER NOT NULL"\
                                            ");"
#define INSERT_INTO_SYS_MAPDB_NAME_Q          "INSERT INTO %q VALUES('%q');"
#define SELECT_FROM_SYS_MAPDB_NAME_Q          "SELECT MAPNAME FROM %q;"
#define INSERT_INTO_NAME_QQL                "INSERT INTO %q VALUES('%q',%ld);"
#define DELETE_FROM_NAME_WHERE_QQ           "DELETE FROM %q WHERE MAPKEY='%q';"
#define DELETE_FROM_NAME_Q                  "DELETE FROM %q WHERE 1=1;"
#define SELECT_FROM_NAME_WHERE_QQ           "SELECT MAPVAL FROM %q WHERE MAPKEY='%q';"
#define SELECT_COUNT_FROM_NAME_Q            "SELECT COUNT(*) CNT FROM %q;"
#define DROP_TABLE_NAME_Q                   "DROP TABLE %q;"
#define SELECT_FROM_SYS_MAPDB_NAME_WHERE_NAME_QQ  "SELECT MAPNAME FROM %q WHERE MAPNAME='%q';"
#define DELETE_FROM_SYS_MAPDB_NAME_WHERE_NAME_QQ  "DELETE FROM %q WHERE MAPNAME='%q';"

/*---------------------------------------------------------------------------*/
long _mapdb_populate( mapdb_t* map, 
                    char* name, 
                    fn_mapdb_populate_callback callback, 
                    const void* userdata,
                    int reverse)
{
    int rc = SQLITE_OK;
    void* valp = 0;
    const unsigned char* key;
    char* zSQL;
    sqlite3_stmt* stmt;

    if (!map)
    {
        return SQLITE_ERROR;
    }
    if (0 == callback)
    {
        return rc;
    }
    zSQL = sqlite3_mprintf(
                (reverse != 0 ? SELECT_FROM_NAME_ORDER_MAPKEY_DESC_Q :
                    SELECT_FROM_NAME_ORDER_MAPKEY_ASC_Q), 
                name);
    rc = sqlite3_prepare(map->memdb,
            zSQL,
            -1,
            &stmt,
            0);
    sqlite3_free(zSQL);

    if (SQLITE_OK == rc)
    {
        rc = sqlite3_step(stmt);
        while (SQLITE_ROW == rc)
        {
            valp = (void*)(long)(sqlite3_column_int(stmt, 0));
            key = sqlite3_column_text(stmt, 1);
            if (callback((char*)key, valp, userdata) != 0)
            {
                break;
            }
            rc = sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }

    return rc;
}

long _mapdb_clear_callback(char* key, void* valp, const void* userdata)
{
    _mapdb_node_release(valp);
    return 0;
}

/*---------------------------------------------------------------------------*/

mapdb_t* mapdb_new()
{
    char* zSQL;
    int rc = SQLITE_OK;
    mapdb_t* map = (mapdb_t*)malloc(sizeof(mapdb_t));
    if (!map)
    {
        return 0;
    }
    rc = sqlite3_open(USING_MEMDB, &map->memdb);
    if (rc != SQLITE_OK)
    {
        free(map);
        return 0;
    }

    zSQL = sqlite3_mprintf(
                CREATE_TABLE_SYS_MAPDB_NAME_Q, 
                SYS_MAPDB_NAME);
    rc = sqlite3_exec(map->memdb,
        zSQL,
        0,
        0,
        0);

    sqlite3_free(zSQL);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(map->memdb);
        free(map);
        return 0;
    }
    return map;
}

void  mapdb_del(mapdb_t* map)
{
    if (map != 0)
    {
        _mapdb_free(map);
        free(map);
    }
}

/* map methods */
int mapdb_init(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    char* zSQL;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    /*create the new table*/
    zSQL = sqlite3_mprintf(
                    CREATE_TABLE_NAME_Q, 
                    name);
    rc = sqlite3_exec(map->memdb,
            zSQL,
            0,
            0,
            0);
    sqlite3_free(zSQL);

    if (rc != SQLITE_OK)
    {
        return rc;
    }
    /*insert the new table name into the sys_table*/
    zSQL = sqlite3_mprintf(
                INSERT_INTO_SYS_MAPDB_NAME_Q,
                SYS_MAPDB_NAME, name);
    rc = sqlite3_exec(map->memdb,
            zSQL,
            0,
            0,
            0);
    sqlite3_free(zSQL);
    return rc;
}

int mapdb_release(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    char* zSQL;

    if (!map)
    {
        return SQLITE_ERROR;
    }
    rc = mapdb_clear(map, name);/*free all allocated items*/
    if (SQLITE_OK == rc)
    {
        /*drop the specific table name created by mapdb_init()*/
        zSQL = sqlite3_mprintf(
                    DROP_TABLE_NAME_Q,
                    name);
        rc = sqlite3_exec(map->memdb,
                zSQL,
                0,
                0,
                0);
        sqlite3_free(zSQL);
    }
    if (SQLITE_OK == rc)
    {
        _mapdb_free_name(map, name);/*delete the specific table name from sys_table*/
    }

    return rc;
}

int _mapdb_free_name(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    int found = 0;
    char* zSQL;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    /* find the key if it is existing */
    found = mapdb_exist(map, name);
    /* if found */
    if (!found)
    {
        return SQLITE_NOTFOUND;
    }
    zSQL = sqlite3_mprintf(
            DELETE_FROM_SYS_MAPDB_NAME_WHERE_NAME_QQ, 
            SYS_MAPDB_NAME,
            name);
    rc = sqlite3_exec(map->memdb, zSQL, 0, 0, 0);
    sqlite3_free(zSQL);
    return rc;
}
int _mapdb_free(mapdb_t* map)
{
    int rc = SQLITE_OK;
    sqlite3_stmt* stmt;
    char* zSQL;

    if (!map || 0 == map->memdb)
    {
        return rc;
    }
    zSQL = sqlite3_mprintf(
                SELECT_FROM_SYS_MAPDB_NAME_Q, 
                SYS_MAPDB_NAME);
    rc = sqlite3_prepare(map->memdb,
            zSQL,
            -1,
            &stmt,
            0);

    sqlite3_free(zSQL);

    if (SQLITE_OK == rc)
    {
        rc = sqlite3_step(stmt);
        while (SQLITE_ROW == rc)
        {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            mapdb_release(map, (char*)name);
            rc = sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }
        
    rc = sqlite3_close(map->memdb);
    map->memdb = 0;

    return rc;
}

void* _mapdb_node_init(const void* valp, unsigned long size)
{
    void* newnode = malloc(size);
    if (newnode)
    {
        memcpy(newnode, valp, size);
    }
    return newnode;
}

int mapdb_insert(mapdb_t* map, char* name, char* key, const void* valp, unsigned long size)
{
    int rc = SQLITE_OK;
    char *zSQL;
    void* iter = 0;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    iter = _mapdb_node_init(valp, size);
    if (0 == iter)
    {
        return SQLITE_NOMEM;
    }
    zSQL = sqlite3_mprintf(
                INSERT_INTO_NAME_QQL, 
                name, key, (long)iter);

    rc = sqlite3_exec(map->memdb, zSQL, 0, 0, 0);
    sqlite3_free(zSQL);
    return rc;
}

void _mapdb_node_release(void* node)
{
    free(node);
}

int mapdb_erase(mapdb_t* map, char* name, char* key)
{
    int rc = SQLITE_OK;
    void* iter = 0;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    /* find the key if it is existing */
    iter = mapdb_find(map, name, key);
    /* if found */
    if (iter != 0)
    {
        char* zSQL = sqlite3_mprintf(
                DELETE_FROM_NAME_WHERE_QQ, 
                name, key);
        rc = sqlite3_exec(map->memdb, zSQL, 0, 0, 0);
        sqlite3_free(zSQL);

        _mapdb_node_release(iter);
    }
    else
    {
        rc = SQLITE_NOTFOUND;
    }
    return rc;
}

int mapdb_clear(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    char* zSQL;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    /*release all void* objects*/
    rc = mapdb_populate(map, name, _mapdb_clear_callback, 0);

    zSQL = sqlite3_mprintf(
                    DELETE_FROM_NAME_Q,
                    name);
    rc = sqlite3_exec(map->memdb, zSQL, 0, 0, 0);
    sqlite3_free(zSQL);
    return rc;
}

void* mapdb_find(mapdb_t* map, char* name, char* key)
{
    int rc = SQLITE_OK;
    void* valp = 0;
    sqlite3_stmt* stmt;
    char* zSQL;
    if (!map)
    {
        return valp;
    }
    zSQL = sqlite3_mprintf(
                    SELECT_FROM_NAME_WHERE_QQ, 
                    name, key);

    rc = sqlite3_prepare(map->memdb,
            zSQL,
            -1,
            &stmt,
            0);
    sqlite3_free(zSQL);

    if (SQLITE_OK == rc)
    {
        rc = sqlite3_step(stmt);
        if (SQLITE_ROW == rc)
        {
            valp = (void*)(long)(sqlite3_column_int(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return valp;
}

long mapdb_count(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    long cnt = 0;
    sqlite3_stmt* stmt;
    char* zSQL;
    if (!map)
    {
        return SQLITE_ERROR;
    }
    zSQL = sqlite3_mprintf(
                    SELECT_COUNT_FROM_NAME_Q, 
                    name);
    rc = sqlite3_prepare(map->memdb,
            zSQL,
            -1,
            &stmt,
            0);
    sqlite3_free(zSQL);

    if (SQLITE_OK == rc)
    {
        rc = sqlite3_step(stmt);
        if (SQLITE_ROW == rc)
        {
            cnt = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        /* something errors */
        cnt = -1;
    }
    return cnt;
}

long mapdb_populate(
    mapdb_t* map,
    char* name,
    fn_mapdb_populate_callback callback,
    const void* userdata)
{
    return _mapdb_populate(map, name, callback, userdata, 0); /* 0 = not reverse */
}

long mapdb_reverse_populate(
    mapdb_t* map,
    char* name,
    fn_mapdb_populate_callback callback,
    const void* userdata)
{
    return _mapdb_populate(map, name, callback, userdata, 1); /* 1 = reverse */
}

int mapdb_exist(mapdb_t* map, char* name)
{
    int rc = SQLITE_OK;
    sqlite3_stmt* stmt;
    char* zSQL;
    int found = 0;  /*not found*/

    if (!map || 0 == map->memdb)
    {
        return found;
    }
    zSQL = sqlite3_mprintf(
                SELECT_FROM_SYS_MAPDB_NAME_WHERE_NAME_QQ, 
                SYS_MAPDB_NAME,
                name);
    rc = sqlite3_prepare(map->memdb,
            zSQL,
            -1,
            &stmt,
            0);
    sqlite3_free(zSQL);

    if (SQLITE_OK == rc)
    {
        rc = sqlite3_step(stmt);
        while (SQLITE_ROW == rc)
        {
            found = 1;  /*found the name*/
            break;
        }
        sqlite3_finalize(stmt);
    }

    return found;
}
#ifdef __cplusplus
}
#endif
