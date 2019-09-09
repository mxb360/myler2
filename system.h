#ifndef __SYSTEM_H__
#define __SYSTEM_H__

int file_exists(const char *file_name);
int create_dir(const char *dir);
int get_files(char *files[], int max_len, const char *file_name);
char *join_path(char *path1, const char *path2);


#endif
