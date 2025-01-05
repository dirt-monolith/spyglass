#ifndef SPYGLASS_H
#define SPYGLASS_H

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    SPYGLASS_LOG_ERROR,
    SPYGLASS_LOG_WARNING,
    SPYGLASS_LOG_INFO
} spyglass_log_level;

void spyglass_log_print(spyglass_log_level level, const char* format, ...);

#ifdef DEBUG
    #define SPYGLASS_LOG_ERROR(fmt, ...) spyglass_log_print(SPYGLASS_LOG_ERROR, fmt, ##__VA_ARGS__)
    #define SPYGLASS_LOG_WARN(fmt, ...) spyglass_log_print(SPYGLASS_LOG_WARNING, fmt, ##__VA_ARGS__)
    #define SPYGLASS_LOG_INFO(fmt, ...) spyglass_log_print(SPYGLASS_LOG_INFO, fmt, ##__VA_ARGS__)
#else
    #define SPYGLASS_LOG_ERROR(fmt, ...) ((void)0)
    #define SPYGLASS_LOG_WARN(fmt, ...) ((void)0)
    #define SPYGLASS_LOG_INFO(fmt, ...) ((void)0)
#endif

#endif