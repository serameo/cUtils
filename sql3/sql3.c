/*
File name: sql3.c
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a simple function using sqlite3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "sql3.h"


#ifdef __cplusplus
extern "C"
{
#endif
static int   _sql3_open(sql3_t* sql3, const char* sql3file, int _flags);
static int sql3_errno = SQL3_OK;

int sql3_last_errno()
{
    return sql3_errno;
}

sql3_t* sql3_new()
{
    sql3_t* sql3 = (sql3_t*)malloc(sizeof(sql3_t));
    sql3_errno = SQL3_OK;
    if (sql3)
    {
        sql3->db = 0;
    }
    else
    {
        sql3_errno = SQL3_EMEM;
    }
    return sql3;
}

void sql3_del(sql3_t* sql3)
{
    sql3_errno = SQL3_OK;
    if (sql3)
    {
        sql3_close(sql3);
        free(sql3);
    }
}

int sql3_open(sql3_t* sql3, const char* sql3file)
{
    return _sql3_open(sql3, sql3file, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

int sql3_open_read(sql3_t* sql3, const char* sql3file)
{
    return _sql3_open(sql3, sql3file, SQLITE_OPEN_READONLY);
}
int   sql3_open2(sql3_t* sql3, const char* sql3file, int _flags)
{
    return _sql3_open(sql3, sql3file, _flags);
}
static int   _sql3_open(sql3_t* sql3, const char* sql3file, int _flags)
{
    int rc = 0;
    sqlite3 *db;

    sql3_errno = SQL3_OK;
    if (sql3->db)
    {
        return rc;
    }
    rc = sqlite3_open_v2(sql3file, &db, _flags, 0);
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        sql3_errno = SQL3_ESQL;
        return rc;
    }
    sql3->db = db;
    return rc;
}

void sql3_close(sql3_t* sql3)
{
    sql3_errno = SQL3_OK;
    if (sql3->db)
    {
        sqlite3_close(sql3->db);
        sql3->db = 0;
    }
}

int sql3_exec(sql3_t* sql3, const char* _sqlcmd, sql3_exec_callback _callback, void* _userptr)
{
    int rc = 0;
    char *errmsg = 0;
    sql3_errno = SQL3_OK;
    if (SQL3_ISNULL(sql3->db))
    {
        sql3_errno = SQL3_ECONSTRUCTOR;
        return SQLITE_ERROR;
    }
    rc = sqlite3_exec(sql3->db, _sqlcmd, _callback, _userptr, &errmsg);
    if (rc != SQLITE_OK)
    {
        sql3_errno = SQL3_ESQL;
        sqlite3_free(errmsg);
    }
    return rc;
}

sql3_sel_t*  sql3_select(sql3_t* sql3, const char* _sqlcmd, sql3_copy_callback _callback, int _size)
{
    /*See sqlite doc*/
    sqlite3_stmt    *stmt;
    int        rc      = 0; 
    int        ncols   = 0;
    int        col     = 0;
    const char*   value   = 0;
    sql3_sel_t*  sel     = 0;
    int   rc2     = 0;
    int     nrows   = 0;
    int     row     = 0;
    void*   item    = 0;
    void*   items   = 0;
    int     done    = SQL3_FALSE;
    int     size    = _size;
    int     nitems  = 0;

    sql3_errno = SQL3_OK;
L000_INIT:
    if (SQL3_ISNULL(_callback) || _size < 1)
    {
        /*required callback and size correctly*/
        sql3_errno = SQL3_ERROR;
        return 0;   /*NULL*/
    }

L100_PREPARE_SQL:
    rc = sqlite3_prepare_v2(sql3->db, _sqlcmd, -1, &stmt, 0);
    /*rc = sqlite3_prepare_v3(sql3->db, _sqlcmd, -1, 0, &stmt, 0);*/
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        sql3_errno = SQL3_ESQL;
        return 0;   /*NULL*/
    }

L200_COUNT_ROWS_RETURNED:
    /*count rows*/
    do
    {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW)
        {
            break;
        }
        ++nrows;
    } while (1);
    sqlite3_reset(stmt);    /*rewind to the first row if it has*/
    
L300_INIT_AN_ARRAY:
    /*create a pointer to an array of records*/
    if (nrows > 0)
    {
        size = _size * nrows;
        items = (void*)malloc(size);
        if (items)
        {
            memset(items, 0, size);
        }
    }
    else
    {
        items   = 0;
        sql3_errno = SQL3_NOROW;
    }

L400_ALLOC_NEW_OBJECT_RETURNED:
    sel = (sql3_sel_t*)malloc(sizeof(sql3_sel_t));
    if (SQL3_ISNULL(sel))
    {
        free(items);
        sql3_errno = SQL3_EMEM;
        return 0;   /*NULL*/
    }
    sel->nrows  = nrows;
    sel->items  = items;
    sel->size   = _size;
    sel->nitems = 0;
    if (0 == nrows)
    {
        sqlite3_finalize(stmt);
        return sel;
    }

L500_COPY_DATA_BY_USER_CALLBACK:
    ncols   = sqlite3_column_count(stmt);
    row     = 0;
    do
    {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW)
        {
            done = SQL3_TRUE;
            break;
        }
        item = (char*)(items) + (row * _size);
        for (col = 0; col < ncols; ++col)
        {
            value = sqlite3_column_text(stmt, col);
            rc2 = _callback(item, col, value);
            if (rc2 != 0)
            {
                done = SQL3_TRUE;
                break;
            }
        }
        ++row;
        ++nitems;
    } while ((row < nrows) && (SQL3_FALSE == done));
    sel->nitems = nitems;
    
L600_RELEASE_SQL_STATEMENT:
    sqlite3_finalize(stmt);

L1000_EXIT_FUNCTION:
    return sel;
}

void* sql3_sel_get(sql3_sel_t* _selptr, int _row)
{
    void* ptr = 0;
    sql3_errno = SQL3_EOUTBOUND;
    if (_row >= 0 && _selptr && _row < _selptr->nitems)
    {
        ptr = (void*)((char*)(_selptr->items) + (_row * _selptr->size));
        sql3_errno = SQL3_OK;
    }
    return ptr;
}

void sql3_sel_free(sql3_sel_t* _selptr)
{
    sql3_errno = SQL3_OK;
    if (_selptr)
    {
        free(_selptr->items);
        free(_selptr);
    }
}

double sql3_aggregate(sql3_t* sql3, const char* _sqlcmd)
{
    sqlite3_stmt *stmt;
    double agg = 0.0;
    int rc = SQLITE_OK;

    sql3_errno = SQL3_OK;
    rc = sqlite3_prepare_v2(sql3->db, _sqlcmd, -1, &stmt, 0);
    /*rc = sqlite3_prepare_v3(sql3->db, _sqlcmd, -1, 0, &stmt, 0);*/
    if (rc != SQLITE_OK)
    {
        sql3_errno = SQL3_ESQL;
        return 0;
    }
    do
    {
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW)
        {
            sql3_errno = SQL3_NOROW;
            break;
        }
        agg = sqlite3_column_double(stmt, 0);
    } while (0);
    
    return agg;
}


static int sql3__has_table(void* args, int argc, char** argv, char** colname) /*ETSDEV-794*/
{
    int* row = (int*)args;
    *row = atoi(argv[0]);
    return SQLITE_OK;
}

int     sql3_is_existing(sql3_t* sql3, const char* _tabname)
{
    /*see sqlite3 doc*/
    char  zSql[BUFSIZ];
    const char* sql = "SELECT COUNT(type) FROM sqlite_master WHERE type = 'table' AND name = '%s'";
    char *zErrMsg = 0;
    int rc = SQLITE_ERROR;
    int row = -1;

    int fExisting = SQL3_FALSE;

    sqlite3_snprintf(BUFSIZ, zSql, sql, _tabname);
    rc = sqlite3_exec(sql3->db, zSql, sql3__has_table, &row, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(zErrMsg);
    }
    fExisting = (row > 0);
    return fExisting;
}


#ifdef __cplusplus
}
#endif
