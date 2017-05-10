#include <stdio.h>

char* trim(char* str)
{
    char *begin = str;
    char *end;
    if(begin != NULL) {

        end = begin + strlen(str) - 1;
        while (*begin && isspace(*begin)) {
            begin++;
        }
        while ( end > begin && isspace(*end)) {
            *end-- = '\0';
        }
    }
    return begin;
}


