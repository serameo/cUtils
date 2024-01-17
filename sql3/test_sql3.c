#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sql3.h"

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
