#include "spyglass.h"
#include "spyglass_internal.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

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

static int s_log_add_source_location(char* buffer, size_t buffer_size, __attribute__((unused)) const char* file, const char* func, int line)
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

static int s_log_ensure_log_directory() {
    struct stat st = {0};
    if (stat(SPYGLASS_LOG_DIR, &st) == -1) {
        #ifdef _WIN32
        return mkdir(SPYGLASS_LOG_DIR);
        #else
        return mkdir(SPYGLASS_LOG_DIR, 0700);
        #endif
    }
    return 0;
}

static void s_log_init_file()
{
    if (s_log_file) return;
    
    if (s_log_ensure_log_directory() != 0) {
        fprintf(stderr, "Failed to create log directory: %s\n", strerror(errno));
        return;
    }

    char log_path[256];
    time_t now;
    struct tm* tm_info;
    time(&now);
    tm_info = localtime(&now);
    
    snprintf(log_path, sizeof(log_path), "%s/log_%04d%02d%02d_%02d%02d%02d.txt",
             SPYGLASS_LOG_DIR,
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
             
    s_log_file = fopen(log_path, "a");
    if (!s_log_file) 
    {
        fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
    }
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

    fprintf(SPYGLASS_LOG_OUTPUT, "%s%s", color, prefix_buffer);
    va_list args;
    va_start(args, format);
    vfprintf(SPYGLASS_LOG_OUTPUT, format, args);
    va_end(args);
    fprintf(SPYGLASS_LOG_OUTPUT, "%s\n", reset_color);

    #if (SPYGLASS_CONFIG_FLAGS & SPYGLASS_LOG_CFG_USE_FILE_BIT)
        if (!s_log_file) s_log_init_file();
        if (s_log_file) {
            va_list args2;
            va_start(args2, format);
            fprintf(s_log_file, "%s", prefix_buffer);
            vfprintf(s_log_file, format, args2);
            fprintf(s_log_file, "\n");
            fflush(s_log_file);
            va_end(args2);
        }
    #endif
}

void s_log_cleanup(void) {
    if (s_log_file) {
        fclose(s_log_file);
        s_log_file = NULL;
    }
}