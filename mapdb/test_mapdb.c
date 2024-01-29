#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapdb.h"

int cmpname(void* p1, void* p2, void* userdata)
{
    char* n1 = (char*)p1;
    char* n2 = (char*)p2;
    return strcmp(n1, n2);
}

struct student_s
{
    int     id;
    char    name[32];
    int     grade;
};
typedef struct student_s student_t;

int main()
{
    int rc = 0;
    mapdb_t* map = mapdb_new();
    student_t student;
    student_t* iter = 0;
    
    rc = mapdb_init(map, "student");
    
    student.id = 1;
    student.grade = 6;
    strcpy(student.name, "meo");    
    mapdb_insert(map, "student", student.name, &student, sizeof(student));
    
    student.id = 2;
    student.grade = 2;
    strcpy(student.name, "matt");    
    mapdb_insert(map, "student", student.name, &student, sizeof(student));
    
    student.id = 3;
    student.grade = 5;
    strcpy(student.name, "knij");    
    mapdb_insert(map, "student", student.name, &student, sizeof(student));
    
    student.id = 4;
    student.grade = 1;
    strcpy(student.name, "malte");    
    mapdb_insert(map, "student", student.name, &student, sizeof(student));
 

    iter = (student_t*)mapdb_find(map, "student", "meo");
    if (iter)
    {
        printf("id=%d, name=%s, grade=%d\n", iter->id, iter->name, iter->grade);
    }
    else
    {
        printf("not found\n");
    }

    mapdb_del(map);
    return 0;
}
