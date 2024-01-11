#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xcnf.h"

int main()
{
    char* value;
    xcnf_t* xcnf = xcnf_new();
    FILE* fp = fopen("./abc.txt", "r+");
    int rc = xcnf_load(xcnf, fp);
    fclose(fp);
    
    if (0 != rc)
    {
        printf("err: %s\n", xcnf_get_errmsg());
        return rc;
    }
    
    value = xcnf_get(xcnf, "section1", "screen1.width", "undeclared");
    printf("value = '%s'\n", value);
    return 0;
}
