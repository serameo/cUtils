/*
File name: sql3.h
Author: Seree Meo Rakwong
Date: 17-JAN-2024
Purpose: Implement a simple function using sqlite3
*/

#ifndef __SQL3_H__
#define __SQL3_H__

#include <sqlite3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SQL3_BREAK        ( 2)
#define SQL3_NOROW        ( 1)
#define SQL3_OK           ( 0)
#define SQL3_ERROR        (-1)
#define SQL3_EMEM         (-2)
#define SQL3_ESQL         (-3)
#define SQL3_ECONSTRUCTOR (-4)
#define SQL3_EOUTBOUND    (-5)

#define SQL3_FAILED(e)    ((e) <  0)
#define SQL3_SUCCEEDED(e) ((e) >= 0)

#define SQL3_TRUE         (1)
#define SQL3_FALSE        (0)

#define SQL3_NULL         ((void*)0)
#define SQL3_ISNULL(p)    (!(p))

struct sql3_s
{
    sqlite3 *db;
};
typedef struct sql3_s sql3_t;

sql3_t*         sql3_new();
void            sql3_del(sql3_t* sql3);

int     sql3_open(sql3_t* sql3, const char* dbfile);
int     sql3_open2(sql3_t* sql3, const char* dbfile, int flags);
int     sql3_open_read(sql3_t* sql3, const char* dbfile);
void            sql3_close(sql3_t* sql3);

/*****************************************************************************
CREATE TABLE student(
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    name    TEXT,
    age     REAL,
    height  REAL,
    weight  REAL
    );

INSERT INTO student(name,age,height,weight) VALUES('smith',12.5,157.2,58.2);
INSERT INTO student(name,age,height,weight) VALUES('neo',12.3,158.2,52.3);
INSERT INTO student(name,age,height,weight) VALUES('trinity',12.6,162.2,55.8);
INSERT INTO student(name,age,height,weight) VALUES('morphoues',13.5,163.9,65.3);
INSERT INTO student(name,age,height,weight) VALUES('oracle',33.8,160.9,60.3);
INSERT INTO student(name,age,height,weight) VALUES('tank',10.5,152.9,45.3);

UPDATE student SET name='smith' WHERE id=1;

DELETE student WHERE id=6;
*****************************************************************************/
/* See sqlite3 document*/
typedef int (*sql3_exec_callback)(void* userptr, int argc, char **argv, char **name);
int    sql3_exec(sql3_t* db, const char* sqlcmd, sql3_exec_callback callback, void* userptr);

/*****************************************************************************
Example to call copy from callback function
struct _student
{
    long    id;
    char    name[32];
    double  age;
    double  height;
    double  weight;
};
typedef struct _student student_t;

int copy_student_callback(void* userptr, int col, const char* value)
{
    student_t *student = (student_t*)userptr;
    char* val = (char*)value;
    switch(col)
    {
        case 1: strcpy(student->name, (val ? val   : ""));      break;
        case 0: student->id     = (val ? atol(val) : 0);        break;
        case 2: student->age    = (val ? atof(val) : 0);        break;
        case 3: student->height = (val ? atof(val) : 0);        break;
        case 4: student->weight = (val ? atof(val) : 0);        break;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    const char *sqlcmd = "SELECT * FROM student WHERE AGE > 7;";
    sql3_t*  db = sql3_new();
    int rc = sql3_open(db, "./student.db");
    if (SQL3_OK != rc)
    {
        return 1;
    }

    sql3_sel_t* selptr = sql3_select(db, sqlcmd, copy_student_callback, sizeof(student_t));
    if (selptr)
    {
        if (selptr->nitems > 0 && selptr->items)
        {
            int rows = selptr->nitems;
            student_t *student = 0;
            int i = 0;
            for (i = 0; i < rows; ++i)
            {
                student = (student_t *)sql3_sel_get(selptr, i);
                printf("id:%ld, name:'%s', height: %f, weight: %f\n",
                    student->id, student->name, student->height, student->weight);
            }
        }
        else
        {
            printf("data not found\n");
        }
    }
    else
    {
        printf("statement is incorrect or table not found\n");
    }

    sql3_close(db);
    sql3_del(db);
    return 0;
}
*****************************************************************************/
/*
int sql3_copy_callback(P1, P2, P3)
PURPOSE:
    To copy a row data from DB by calling sql3_select()

RETURN:
    SQL3_OK to continue copy data from DB. Otherwise break the copy from DB.

PARAMETERS:
    P1 - to copy for each row from DB
    P2 - the DB column index by the zero based
    P3 - the DB value for each column returned by selecting
*/
typedef int (*sql3_copy_callback)(void* userptr, int col, const char* value);

struct sql3_select_s
{
    int             size;   /*size of an item per record*/
    int             nrows;  /*a number of rows*/
    void*           items;  /*an array of items*/
    int             nitems; /*an actual items, it may not be equal nrows*/
                            /*it depends on the implementation of callback*/
                            /*it always not be greater than nrows*/
};
typedef struct sql3_select_s sql3_sel_t;

/*****************************************************************************
sql3_select() would return an array of records
SQL Example:
    const char *sqlcmd = "SELECT * FROM STUDENT_TABLE WHERE AGE > 7;";

Return:
    sql3_sel_t* if sqlcmd was successfully called. Please double-check nrows and items if there
    is no row returned.
    Otherwise, NULL if sqlcmd is incorrect statement. Such as no table in database and so on.

Example:
    sql3_sel_t* selptr = sql3_select(db, sqlcmd, copy_student_callback, sizeof(student_t));
    if (selptr)
    {
        if (selptr->nitems > 0 && selptr->items)
        {
            printf("data found\n");
        }
        else
        {
            printf("data not found\n");
        }
    }
    else
    {
        printf("statement is incorrect or table not found\n");
    }
*****************************************************************************/
sql3_sel_t*  sql3_select(sql3_t* db, const char* sqlcmd, sql3_copy_callback callback, int size);
/*
sql3_sel_get() to read a specific record with a zero base
Example:
    student_t *student = sql3_sel_get(selptr, 0);
*/
void*   sql3_sel_get(sql3_sel_t* selptr, int row);
/*
sql3_sel_free() to release sql3_sel_t* object
*/
void    sql3_sel_free(sql3_sel_t* selptr);
/*****************************************************************************
sql3_aggregate() would only one aggregate SQL function
SQL Example:
    const char *sqlcmd1 = "SELECT COUNT(*) FROM student;";
    const char *sqlcmd2 = "SELECT AVG(age) FROM student;";
    const char *sqlcmd3 = "SELECT MAX(age) FROM student;";
    const char *sqlcmd4 = "SELECT SUM(age) FROM student;";

Example:
    long    count_students  = (long)sql3_aggregate(db, sqlcmd1);
    double  avg_age         = sql3_aggregate(db, sqlcmd2);
    double  max_age         = sql3_aggregate(db, sqlcmd3);
    double  sum_age         = sql3_aggregate(db, sqlcmd4);
*****************************************************************************/
double  sql3_aggregate(sql3_t* db, const char* sqlcmd);

int     sql3_last_errno();
int     sql3_is_existing(sql3_t* db, const char* tabname);

#ifdef __cplusplus
}
#endif

#endif /* __SQL3_H__ */
