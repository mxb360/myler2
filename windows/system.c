#include "system.h"
#include "mystring.h"

#include <stdlib.h>
#include <io.h>
#include <direct.h>

int file_exists(const char *file_name)
{
    return !_access(file_name, 0);
}

int create_dir(const char *dir)
{
//    char *buf[50];
//    char dir_buf[256];

    return !_mkdir(dir);
}

int get_files(char *files[], int n, const char *dir)
{
    struct _finddata_t file;
    char buf[256];
    long handle;
    int i, total = 0;
    char *path = NULL, *path_bak = _getcwd(NULL, 0);

    strcpy(buf, dir);
    for (i = strlen(buf); i >= 0 && buf[i] != '\\' && buf[i] != '/'; i--);
    if (i >= 0)
    {
        buf[i] = 0;
        if (_chdir(buf) < 0) {
            free(path_bak);
            return -1;
        }
    }  
    path = _getcwd(NULL, 0);
    _chdir(path_bak);
    free(path_bak);

    handle = _findfirst(dir, &file);
    if (handle == -1) {
        free(path);
        return -1;
    }
    int a = 0;
    do {
        files[total] = (char *)malloc(strlen(file.name) + strlen(path) + 10);
        if (files[total] != NULL && file.name[0] != '.') {
            strcpy(files[total], path);
            strcat(files[total], "\\");
            strcat(files[total++], file.name);
        }
    } while (total < n && !(a = _findnext(handle, &file)));
    
    free(path);
    return total;
}

char *join_path(char *path1, const char *path2)
{
    int i, j = 0;

    for (i = 0; path1[i]; i++);
    if (i > 1 && path1[i - 1] != '\\') {
        path1[i] = '\\';
        i++;
    }
    if (path2[0] == '\\')
        j++;

    while (path2[j])
        path1[i++] = path2[j++];
    path1[i] = 0;
    return path1;
}
