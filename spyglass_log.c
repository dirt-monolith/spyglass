#include "spyglass.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>

// ----- PRIVATE global config -----
static spyglass_log_config config_cur =
{
    .active_flags = SPYGLASS_LOG_CFG_SHOW_TIME | SPYGLASS_LOG_CFG_SHOW_FILE | SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_LINE,
    .active_levels = SPYGLASS_LOG_ERROR
};

// ----- PRIVATE HELPER IMPLEMENTATIONS -----
typedef struct 
{
    const char* color;
    const char* reset_color;
    const char* level_str;
} log_level_config;

static int format_log_prefix(char* buffer, size_t buffer_size, const log_level_config* level_cfg, const char* file, const char* func, int line);
static int add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line);

static int format_log_prefix(char* buffer, size_t buffer_size, const log_level_config* level_cfg, const char* file, const char* func, int line)
{
    int offset = 0;

    if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_TIME)
    {
        time_t now;
        struct tm* tm_info;
        time(&now);
        tm_info = localtime(&now);
        offset += strftime(buffer + offset, buffer_size - offset, "[%Y-%m-%d %H:%M:%S] ", tm_info);
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "%s ", level_cfg->level_str);

    if (config_cur.active_flags & (SPYGLASS_LOG_CFG_SHOW_FILE | SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_LINE))
    {
        offset += add_source_location(buffer + offset, buffer_size - offset, file, func, line);
    }

    return offset;
}

static int add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line)
{
    int offset = 0;
    offset += snprintf(buffer + offset, buffer_size - offset, "(");
    
    if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FILE) 
    {
        const char* filename = strrchr(file, '/');
        filename = filename ? filename + 1 : file;
        offset += snprintf(buffer + offset, buffer_size - offset, "%s", filename);
    }
    
    if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FUNC) 
    {
        const char* separator = (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_FILE) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%s", separator, func);
    }
    
    if (config_cur.active_flags & SPYGLASS_LOG_CFG_SHOW_LINE) 
    {
        const char* separator = (config_cur.active_flags & (SPYGLASS_LOG_CFG_SHOW_FUNC | SPYGLASS_LOG_CFG_SHOW_FILE)) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%d", separator, line);
    }
    
    offset += snprintf(buffer + offset, buffer_size - offset, "): ");
    return offset;
}

// ----- PUBLIC API IMPLEMENTATIONS -----

void spyglass_log_init(spyglass_log_config* config)
{
    if (config) config_cur = *config;
}

void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...) 
{
    if (!(config_cur.active_levels & level)) return;

    static const log_level_config log_levels[] = {
        [SPYGLASS_LOG_ERROR]   = {SPYGLASS_LOG_COLOR_ERROR, SPYGLASS_LOG_COLOR_RESET, "[ERROR]"},
        [SPYGLASS_LOG_WARNING] = {SPYGLASS_LOG_COLOR_WARN, SPYGLASS_LOG_COLOR_RESET, "[WARNING]"},
        [SPYGLASS_LOG_INFO]    = {SPYGLASS_LOG_COLOR_INFO, SPYGLASS_LOG_COLOR_RESET, "[INFO]"}
    };

    const unsigned int use_colors = config_cur.active_flags & SPYGLASS_LOG_CFG_USE_COLOR;
    const char* color = use_colors ? log_levels[level].color : "";
    const char* reset_color = use_colors ? log_levels[level].reset_color : "";

    char prefix_buffer[512];
    format_log_prefix(prefix_buffer, sizeof(prefix_buffer), &log_levels[level], file, func, line);

    fprintf(stderr, "%s%s", color, prefix_buffer);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "%s\n", reset_color);
}