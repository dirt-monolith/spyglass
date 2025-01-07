#include "spyglass.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

static int use_color = 1;

void spyglass_debug_init(int use_colors)
{
    use_color = use_colors;
}

void spyglass_log_print(spyglass_log_level level, const char* file, const char* func, int line, const char* format, ...) 
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    char file_copy[256];
    strncpy(file_copy, file, sizeof(file_copy)-1);
    const char* filename = basename(file_copy);

    const char* color = "";
    const char* level_str = "";

    if (use_color) {
        switch(level) {
            case SPYGLASS_LOG_ERROR:
                color = SPYGLASS_LOG_COLOR_ERROR;
                level_str = "ERROR";
                break;
            case SPYGLASS_LOG_WARNING:
                color = SPYGLASS_LOG_COLOR_WARN;
                level_str = "WARNING";
                break;
            case SPYGLASS_LOG_INFO:
                color = SPYGLASS_LOG_COLOR_INFO;
                level_str = "INFO";
                break;
        }
    }

    fprintf(stderr, "%s[%s] %s (%s:%s:%d): ", color, timestamp, level_str, filename, func, line);

    va_list args;
    va_start(args, format);
    fprintf(stderr, "%s", color);
    vfprintf(stderr, format, args);
    va_end(args);

    if (use_color) {
        fprintf(stderr, "%s", SPYGLASS_LOG_COLOR_RESET);
    }
    fprintf(stderr, "\n");
}