#ifndef SPYGLASS_H
#define SPYGLASS_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define SPYGLASS_LOG_COLOR_ERROR "\x1b[31m"
#define SPYGLASS_LOG_COLOR_WARN  "\x1b[33m"
#define SPYGLASS_LOG_COLOR_INFO  "\x1b[34m"
#define SPYGLASS_LOG_COLOR_RESET "\x1b[0m"

typedef enum 
{
    SPYGLASS_LOG_ERROR   = 1 << 0,
    SPYGLASS_LOG_WARNING = 1 << 1,
    SPYGLASS_LOG_INFO    = 1 << 2
} spyglass_log_level;

typedef enum
{
    SPYGLASS_LOG_CFG_USE_COLOR = 1 << 0,
    SPYGLASS_LOG_CFG_SHOW_TIME = 1 << 1,
    SPYGLASS_LOG_CFG_SHOW_FILE = 1 << 2,
    SPYGLASS_LOG_CFG_SHOW_FUNC = 1 << 3,
    SPYGLASS_LOG_CFG_SHOW_LINE = 1 << 4,
} spyglass_log_config_flags;

typedef struct 
{
    unsigned int active_flags;
    unsigned int active_levels;
} spyglass_log_config;

void spyglass_log_init(spyglass_log_config* config);
void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...);

#ifdef DEBUG
    #define SPYGLASS_LOG_ERROR(fmt, ...) spyglass_log_print(SPYGLASS_LOG_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
    #define SPYGLASS_LOG_WARN(fmt, ...) spyglass_log_print(SPYGLASS_LOG_WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
    #define SPYGLASS_LOG_INFO(fmt, ...) spyglass_log_print(SPYGLASS_LOG_INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
    #define SPYGLASS_LOG_ERROR(fmt, ...) ((void)0)
    #define SPYGLASS_LOG_WARN(fmt, ...) ((void)0)
    #define SPYGLASS_LOG_INFO(fmt, ...) ((void)0)
#endif

#endif