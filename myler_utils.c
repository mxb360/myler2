#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>

#include <direct.h>
#include <stdlib.h>

#include "myler_utils.h"
#include "console.h"

void myler_abort(int line, const char *func, const char *file, const char *op)
{
    fprintf(stderr, "[%s: %d] Error: In function \"%s\": \"%s\" Assert Faild.\n", file, line, func, op);
    //*(int *)0 = 0;
    exit(-1);
}

void myler_print_error(bool show_help, const char *format, ...)
{
    char buf[MAX_STR_BUF];
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);

    fprintf(myler_stderr, "myler: ");
    con_set_fcolor(LRED);
    fprintf(myler_stderr, "错误：");
    con_reset_color();
    fprintf(myler_stderr, "%s。\n", buf);
    if (show_help)
        fprintf(myler_stderr, "myler: 尝试“myler --help”查看帮助。\n");
}

void myler_print_warning(const char *format, ...)
{
    char buf[MAX_STR_BUF];
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);

    fprintf(myler_stderr, "myler: ");
    con_set_fcolor(PURPLE);
    fprintf(myler_stderr, "警告：");
    con_reset_color();
    fprintf(myler_stderr, "%s。\n", buf);

}
