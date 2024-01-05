#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"

struct student_s
{
    int id;
    char name[32];
};

int testdata[] = {
    20, 16, 17, 13, 3, 6, 1, 8, 2, 4, 10, 19, 5, 9, 12, 15, 18,
    7, 11, 14
};
/*
int student_cmp(void* p1, void* p2)
{
    struct student_s* s1 = (struct student_s*)p1;
    struct student_s* s2 = (struct student_s*)p2;
    return (s1->id - s2->id);
}
*/
RBTREE_CMP_INT_DECLARE(student_cmp, struct student_s, id);

struct teacher_s
{
    int id;
    char name[32];
};
/*
int teacher_cmp(void* p1, void* p2)
{
    struct teacher_s* s1 = (struct teacher_s*)p1;
    struct teacher_s* s2 = (struct teacher_s*)p2;
    return strcmp(s1->name, s2->name);
}
*/
RBTREE_CMP_STR_DECLARE(teacher_cmp, struct teacher_s, name);

void print_f(void* p1, int flag, void* userdata)
{
    struct student_s* s1 = (struct student_s*)p1;
    switch (flag)
    {
        case RBTREE_NULL_NODE:/*p1 = nil*/
            printf("nil");
            break;
        case RBTREE_LEAF_NODE:
        case RBTREE_NODE:
            printf("%d", s1->id);
            break;
        case RBTREE_LEFT_BEGIN:/*p1 = nil*/
            printf("(");
            break;
        case RBTREE_RIGHT_BEGIN:/*p1 = nil*/
            printf(",");
            break;
        case RBTREE_RIGHT_END:/*p1 = nil*/
            printf(")");
            break;
    }
}

void print_f2(void* p1, int flag, void* userdata)
{
    struct teacher_s* t1 = (struct teacher_s*)p1;
    switch (flag)
    {
        case RBTREE_NULL_NODE:/*p1 = nil*/
            printf("nil");
            break;
        case RBTREE_LEAF_NODE:
        case RBTREE_NODE:
            printf("%s", t1->name);
            break;
        case RBTREE_LEFT_BEGIN:/*p1 = nil*/
            printf("(");
            break;
        case RBTREE_RIGHT_BEGIN:/*p1 = nil*/
            printf(",");
            break;
        case RBTREE_RIGHT_END:/*p1 = nil*/
            printf(")");
            break;
    }
}

int
main()
{
    int i;
    struct rbtree_node_s *n;
    struct student_s s1;
    struct teacher_s t1;
    rbtree_t* students = rbtree_new(student_cmp, 0, 0);
    rbtree_t* teachers = rbtree_new(teacher_cmp, 0, 0);

    for (i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
        s1.id = testdata[i];
        rbtree_insert(students, &s1, sizeof(s1));
    }
    
    strcpy(t1.name, "meo");
    rbtree_insert(teachers, &t1, sizeof(t1));

    strcpy(t1.name, "knij");
    rbtree_insert(teachers, &t1, sizeof(t1));

    strcpy(t1.name, "anchan");
    rbtree_insert(teachers, &t1, sizeof(t1));
    
    strcpy(t1.name, "tor");
    rbtree_insert(teachers, &t1, sizeof(t1));

    rbtree_print(students, rbtree_get_root(students), print_f, 0);
    
    printf("\n\n");

    rbtree_print(teachers, rbtree_get_root(teachers), print_f2, 0);

    printf("\n");
    return (0);
}
