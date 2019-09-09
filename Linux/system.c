#include "system.h"

#include <unistd.h>

int file_exists(const char *file_name)
{
    return access(file_name, 0);
}

int get_files(char *files[], int max_len, const char *file_name)
{

}