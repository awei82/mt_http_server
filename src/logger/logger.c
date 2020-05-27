#ifndef LOGGER_C
#define LOGGER_C

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "logger.h"

static const char *level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

// struct logger_t {
//   FILE *out;
//   int level;
// } logger_t;

// struct logger_t L = { stderr, LOG_TRACE };

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif

void log_log(int level, const char *file, int line, const char *fmt, ...) {
    // if (level < L.level) {
    //     return;
    // }

    /* Get current time */
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    char time_str[64];
    assert(strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", lt));

    /* Log to stderr */
    va_list args;
#ifdef LOG_USE_COLOR
    fprintf(stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
      time_str, level_colors[level], level_names[level], file, line);
#else
    fprintf(stderr, "%s %-5s %s:%d: ", time_str, level_names[level], file, line);
#endif
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

#endif