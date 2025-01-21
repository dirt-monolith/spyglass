#ifndef SPYGLASS_H
#define SPYGLASS_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define SPYGLASS_LOG_COLOR_ERROR "\x1b[31m"
#define SPYGLASS_LOG_COLOR_WARN  "\x1b[33m"
#define SPYGLASS_LOG_COLOR_INFO  "\x1b[34m"
#define SPYGLASS_LOG_COLOR_RESET "\x1b[0m"

#define SPYGLASS_LOG_ERROR_BIT   (1 << 0)
#define SPYGLASS_LOG_WARNING_BIT (1 << 1)
#define SPYGLASS_LOG_INFO_BIT    (1 << 2)

#define SPYGLASS_LOG_CFG_USE_COLOR_BIT (1 << 0)
#define SPYGLASS_LOG_CFG_SHOW_TIME_BIT (1 << 1)
#define SPYGLASS_LOG_CFG_SHOW_FILE_BIT (1 << 2)
#define SPYGLASS_LOG_CFG_SHOW_FUNC_BIT (1 << 3)
#define SPYGLASS_LOG_CFG_SHOW_LINE_BIT (1 << 4)

#ifndef SPYGLASS_CONFIG_FLAGS
#define SPYGLASS_CONFIG_FLAGS (SPYGLASS_LOG_CFG_SHOW_TIME_BIT | SPYGLASS_LOG_CFG_SHOW_FILE_BIT | SPYGLASS_LOG_CFG_SHOW_FUNC_BIT | SPYGLASS_LOG_CFG_SHOW_LINE_BIT)
#endif

#ifndef SPYGLASS_CONFIG_LEVELS
#define SPYGLASS_CONFIG_LEVELS (SPYGLASS_LOG_ERROR_BIT)
#endif

#if (SPYGLASS_CONFIG_LEVELS & SPYGLASS_LOG_ERROR_BIT)
#define SPYGLASS_LOG_ERROR(fmt, ...) spyglass_log_print(SPYGLASS_LOG_ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else 
#define SPYGLASS_LOG_ERROR(fmt, ...) ((void)0)
#endif

#if (SPYGLASS_CONFIG_LEVELS & SPYGLASS_LOG_WARNING_BIT)
#define SPYGLASS_LOG_WARN(fmt, ...) spyglass_log_print(SPYGLASS_LOG_WARNING, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define SPYGLASS_LOG_WARN(fmt, ...) ((void)0)
#endif

#if (SPYGLASS_CONFIG_LEVELS & SPYGLASS_LOG_INFO_BIT)
#define SPYGLASS_LOG_INFO(fmt, ...) spyglass_log_print(SPYGLASS_LOG_INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define SPYGLASS_LOG_INFO(fmt, ...) ((void)0)
#endif

typedef enum 
{
    SPYGLASS_LOG_ERROR   = SPYGLASS_LOG_ERROR_BIT,
    SPYGLASS_LOG_WARNING = SPYGLASS_LOG_WARNING_BIT,
    SPYGLASS_LOG_INFO    = SPYGLASS_LOG_INFO_BIT
} spyglass_log_level;

typedef enum
{
    SPYGLASS_LOG_CFG_USE_COLOR = SPYGLASS_LOG_CFG_USE_COLOR_BIT,
    SPYGLASS_LOG_CFG_SHOW_TIME = SPYGLASS_LOG_CFG_SHOW_TIME_BIT,
    SPYGLASS_LOG_CFG_SHOW_FILE = SPYGLASS_LOG_CFG_SHOW_FILE_BIT,
    SPYGLASS_LOG_CFG_SHOW_FUNC = SPYGLASS_LOG_CFG_SHOW_FUNC_BIT,
    SPYGLASS_LOG_CFG_SHOW_LINE = SPYGLASS_LOG_CFG_SHOW_LINE_BIT,
} spyglass_log_config_flags;

typedef struct 
{
    unsigned int active_flags;
    unsigned int active_levels;
} spyglass_log_config;

void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...);

#endif