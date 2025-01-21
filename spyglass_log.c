#include "spyglass.h"
#include "spyglass_internal.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>

static int s_log_format_prefix(char* buffer, size_t buffer_size, const s_log_config_level* level_cfg, const char* file, const char* func, int line)
{
    int offset = 0;

    #if (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_SHOW_TIME_BIT)
        time_t now;
        struct tm* tm_info;
        time(&now);
        tm_info = localtime(&now);
        offset += strftime(buffer + offset, buffer_size - offset, "[%Y-%m-%d %H:%M:%S] ", tm_info);
    #endif

    offset += snprintf(buffer + offset, buffer_size - offset, "%s ", level_cfg->level_str);

    #if (SPYGLASS_CONFIG_FLAGS & (SPYGLASS_LOG_CFG_SHOW_FILE_BIT | SPYGLASS_LOG_CFG_SHOW_FUNC_BIT | SPYGLASS_LOG_CFG_SHOW_LINE_BIT))
        offset += s_log_add_source_location(buffer + offset, buffer_size - offset, file, func, line);
    #endif

    return offset;
}

static int s_log_add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line)
{
    int offset = 0;
    offset += snprintf(buffer + offset, buffer_size - offset, "(");
    const char* separator;
    
    #if (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_SHOW_FILE_BIT) 
        const char* filename = strrchr(file, '/');
        filename = filename ? filename + 1 : file;
        offset += snprintf(buffer + offset, buffer_size - offset, "%s", filename);
    #endif

    #if (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_SHOW_FUNC_BIT) 
        separator = (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_SHOW_FILE_BIT) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%s", separator, func);
    #endif
    
    #if (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_SHOW_LINE_BIT) 
        separator = (SPYGLASS_CONFIG_FLAGS & (SPYGLASS_LOG_CFG_SHOW_FUNC_BIT | SPYGLASS_LOG_CFG_SHOW_FILE_BIT)) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%d", separator, line);
    #endif
    
    offset += snprintf(buffer + offset, buffer_size - offset, "): ");
    return offset;
}

void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...) 
{
    static const s_log_config_level log_levels[] = {
        [SPYGLASS_LOG_ERROR]   = {SPYGLASS_LOG_COLOR_ERROR, SPYGLASS_LOG_COLOR_RESET, "[ERROR]"},
        [SPYGLASS_LOG_WARNING] = {SPYGLASS_LOG_COLOR_WARN, SPYGLASS_LOG_COLOR_RESET, "[WARNING]"},
        [SPYGLASS_LOG_INFO]    = {SPYGLASS_LOG_COLOR_INFO, SPYGLASS_LOG_COLOR_RESET, "[INFO]"}
    };

    const unsigned int use_colors = (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_USE_COLOR_BIT);
    const char* color = use_colors ? log_levels[level].color : "";
    const char* reset_color = use_colors ? log_levels[level].reset_color : "";

    char prefix_buffer[512];
    s_log_format_prefix(prefix_buffer, sizeof(prefix_buffer), &log_levels[level], file, func, line);

    fprintf(stderr, "%s%s", color, prefix_buffer);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "%s\n", reset_color);
}