#ifndef __MYLER_UTILS_H__
#define __MYLER_UTILS_H__

#include "myler_config.h"

#ifndef MAX
  #define MAX(a, b)     ((a) > (b) ? (a) : (b))
#endif

#ifdef MYLER_DEBUG
  #define myler_assert(op)  ((op) ? (void)0 : myler_abort(__LINE__, __func__, __FILE__, #op))
#else
  #define myler_assert(op)  ((void)0)
#endif

void myler_abort(int line, const char *func, const char *file, const char *op);
void myler_print_error(bool show_help, const char *format, ...);
void myler_print_warning(const char *format, ...);
#endif
