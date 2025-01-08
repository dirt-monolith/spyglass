#include "spyglass.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

static spyglass_log_config config_cur =
{
    .active_flags = SPYGLASS_LOG_CFG_SHOW_TIME | SPYGLASS_LOG_CFG_SHOW_FILE | SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_LINE,
    .active_levels = SPYGLASS_LOG_ERROR
};

void spyglass_log_init(spyglass_log_config* config)
{
    if (config) config_cur = *config;
}

void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...) 
{
    if (!(config_cur.active_levels & level)) return;

    const char* color = "";
    const char* level_str = "";
    unsigned int use_colors = config_cur.active_flags & SPYGLASS_LOG_CFG_USE_COLOR;

    switch(level) {
        case SPYGLASS_LOG_ERROR:
            color = use_colors ? SPYGLASS_LOG_COLOR_ERROR : SPYGLASS_LOG_COLOR_RESET;
            level_str = "ERROR";
            break;
        case SPYGLASS_LOG_WARNING:
            color = use_colors ? SPYGLASS_LOG_COLOR_WARN : SPYGLASS_LOG_COLOR_RESET;
            level_str = "WARNING";
            break;
        case SPYGLASS_LOG_INFO:
            color = use_colors ? SPYGLASS_LOG_COLOR_INFO : SPYGLASS_LOG_COLOR_RESET;
            level_str = "INFO";
            break;
    }

    fprintf(stderr, "%s", color);

    if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_TIME)
    {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(stderr, "[%s] ", timestamp);
    }

    fprintf(stderr, "%s ", level_str);

    if (config_cur.active_flags & (SPYGLASS_LOG_CFG_SHOW_FILE | SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_LINE))
    {
        fprintf(stderr, "(");
        if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FILE)
        {
            char file_copy[256];
            strncpy(file_copy, file, sizeof(file_copy)-1);
            const char* filename = basename(file_copy);

            fprintf(stderr, "%s", filename);
        }
        if ((config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FUNC) && (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FILE))
        {
            fprintf(stderr, ":%s", func);
        }
        else if ((config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FUNC) && !(config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FILE))
        {
            fprintf(stderr, "%s", func);
        }
        if ((config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_LINE) && (config_cur.active_flags & (SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_FILE)))
        {
            fprintf(stderr, ":%d", line);
        }
        else if ((config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_LINE) && !(config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FUNC))
        {
            fprintf(stderr, "%d", line);
        }
        fprintf(stderr, "): ");
    }

    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s", color);
    vfprintf(stderr, format, args);
    va_end(args);

    if (use_colors) {
        fprintf(stderr, "%s", SPYGLASS_LOG_COLOR_RESET);
    }
    fprintf(stderr, "\n");
}