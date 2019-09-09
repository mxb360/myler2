#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *string_strip(char *string)
{
    char *q, *p, *s;

    if (string == NULL)
        return NULL;

    p = q = s =  string;

    while (isspace(*q))
        q++;

    while (*p)
        p++;

    while (isspace(*--p));

    *++p = 0;

    if (q != s) {
        while (q <= p)
            *s++ = *q++;
    }

    return string;
}

int string_split(char *strings[], int n, char *string, const char *sep)
{
    int i = 0;

    strings[i] = strtok(string, sep);
    while (strings[i]) 
        strings[++i] = strtok(NULL, sep);
    return i + 1;
}