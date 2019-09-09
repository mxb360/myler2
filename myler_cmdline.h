#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#include "myler_config.h"

typedef struct myler_cmdline_t {
    int argc;
    char **argv;

    bool have_help;
    bool have_version;
    bool have_exit;
    bool have_mini;
    bool have_stop;
    bool have_order;
    bool have_loop;
    bool have_repeat;
    bool have_shuffle;
    char *words;
    bool have_search;

    int volume;

    const char *music_name[100];
    int music_name_count;
} myler_cmdline_t;

void myler_cmdline_init(myler_cmdline_t *cmdline, int argc, char **argv);

#endif  /* !__CMDLINE_H__ */
