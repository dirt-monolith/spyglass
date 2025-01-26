#include "spyglass.h"
#include "spyglass_internal.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static int s_format_prefix(char* buffer, size_t buffer_size, const s_config_spot* cfg_spot, const char* file, const char* func, int line)
{
    int offset = 0;

    #if (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_TIME)
        time_t now;
        struct tm* tm_info;
        time(&now);
        tm_info = localtime(&now);
        offset += strftime(buffer + offset, buffer_size - offset, "[%Y-%m-%d %H:%M:%S] ", tm_info);
    #endif

    offset += snprintf(buffer + offset, buffer_size - offset, "%s ", cfg_spot->spot_str);

    #if (SPYGLASS_CONFIG_LENS & (SPYGLASS_LENS_FILE | SPYGLASS_LENS_FUNC | SPYGLASS_LENS_LINE))
        offset += s_add_source_location(buffer + offset, buffer_size - offset, file, func, line);
    #endif

    return offset;
}

static int s_add_source_location(char* buffer, size_t buffer_size, __attribute__((unused)) const char* file, const char* func, int line)
{
    int offset = 0;
    offset += snprintf(buffer + offset, buffer_size - offset, "(");
    const char* separator;
    
    #if (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_FILE) 
        const char* filename = strrchr(file, '/');
        filename = filename ? filename + 1 : file;
        offset += snprintf(buffer + offset, buffer_size - offset, "%s", filename);
    #endif

    #if (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_FUNC) 
        separator = (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_FILE) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%s", separator, func);
    #endif
    
    #if (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_LINE) 
        separator = (SPYGLASS_CONFIG_LENS & (SPYGLASS_LENS_FUNC | SPYGLASS_LENS_FILE)) ? ":" : "";
        offset += snprintf(buffer + offset, buffer_size - offset, "%s%d", separator, line);
    #endif
    
    offset += snprintf(buffer + offset, buffer_size - offset, "): ");
    return offset;
}

static int s_ensure_log_directory() {
    struct stat st = {0};
    if (stat(SPYGLASS_AIM, &st) == -1) {
        #ifdef _WIN32
        return mkdir(SPYGLASS_AIM);
        #else
        return mkdir(SPYGLASS_AIM, 0700);
        #endif
    }
    return 0;
}

static void s_init_file()
{
    if (s_log_file) return;
    
    if (s_ensure_log_directory() != 0) {
        fprintf(stderr, "Failed to create log directory: %s\n", strerror(errno));
        return;
    }

    char log_path[256];
    time_t now;
    struct tm* tm_info;
    time(&now);
    tm_info = localtime(&now);
    
    snprintf(log_path, sizeof(log_path), "%s/log_%04d%02d%02d_%02d%02d%02d.txt",
             SPYGLASS_AIM,
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
             
    s_log_file = fopen(log_path, "a");
    if (!s_log_file) 
    {
        fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
    }
}

void spyglass_log(spyglass_spot spot, const char* file, const char* func, int line, const char* format, ...) 
{
    static const s_config_spot cfg_spots[] = {
        [SpyglassMayday  ] = {SPYGLASS_LENS_TINT_MAYDAY , SPYGLASS_LENS_TINT_DEFAULT, "[MAYDAY]"            },
        [SpyglassSighting] = {SPYGLASS_LENS_TINT_SIGHT  , SPYGLASS_LENS_TINT_DEFAULT, "[SIGHTING]"          },
        [SpyglassMark    ] = {SPYGLASS_LENS_TINT_MARK   , SPYGLASS_LENS_TINT_DEFAULT, "[MARK]"              },
        [SpyglassDeploy  ] = {SPYGLASS_LENS_TINT_DEFAULT, SPYGLASS_LENS_TINT_DEFAULT, "[SPYGLASS DEPLOYED]" },
        [SpyglassStow    ] = {SPYGLASS_LENS_TINT_DEFAULT, SPYGLASS_LENS_TINT_DEFAULT, "[SPYGLASS STOWED]"   }
    };

    const unsigned int use_colors = (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_COLOR);
    const char* color = use_colors ? cfg_spots[spot].color : "";
    const char* reset_color = use_colors ? cfg_spots[spot].reset_color : "";

    char prefix_buffer[512];
    s_format_prefix(prefix_buffer, sizeof(prefix_buffer), &cfg_spots[spot], file, func, line);

    fprintf(LOG_OUTPUT, "%s%s", color, prefix_buffer);
    va_list args;
    va_start(args, format);
    vfprintf(LOG_OUTPUT, format, args);
    va_end(args);
    fprintf(LOG_OUTPUT, "%s\n", reset_color);

    #if (SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_AIM)
        if (!s_log_file) s_init_file();
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