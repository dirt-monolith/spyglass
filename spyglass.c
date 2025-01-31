#include "spyglass.h"
#include "spyglass_internal.h"
#include <stdarg.h>
#include <stdlib.h> 
#include <time.h>
#include <string.h>
#include <ctype.h>
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

// TODO(01.27.2025|jared): Eventually package this in it's own library of string manipulations outside of std
static char* string_dup(const char* str) 
{
    if (str == NULL) return NULL;
    size_t len = strlen(str) + 1;  // +1 for null terminator
    char* new_str = malloc(len);
    if (new_str) {
        memcpy(new_str, str, len);
    }
    return new_str;
}

// TODO(01.27.2025|jared): Eventually package this in it's own library for use in files and paths
static char* s_dirname(char* path) 
{
    if (path == NULL) return NULL;
    
    char* last_slash = NULL;
    char* current = path;
    
    while (*current != '\0') {
        if (*current == '/') {
            last_slash = current;
        }
        current++;
    }
    
    if (last_slash == NULL) {
        return ".";
    }
    
    if (last_slash == path) {
        return "/";
    }
    
    *last_slash = '\0';
    
    return path[0] == '\0' ? "/" : path;
}

// TODO(01.27.2025|jared): Eventually package this in it's own library for use in files and paths
static int s_ensure_directory(const char* path) 
{
    struct stat st = {0};
    
    if (stat(path, &st) == 0) {
        // Exists and is a directory
        if (S_ISDIR(st.st_mode)) {
            return 0;
        }
        // Exists but is not a directory
        return -1;
    }
    
    // If errno is not ENOENT, something else went wrong
    if (errno != ENOENT) {
        return -1;
    }
    
    // Create a copy of the path for manipulation
    char* path_copy = string_dup(path);
    if (path_copy == NULL) {
        return -1;
    }
    
    char* parent = s_dirname(path_copy);
    
    int parent_result = 0;
    if (strcmp(parent, ".") != 0 && strcmp(parent, "/") != 0) {
        parent_result = s_ensure_directory(parent);
    }
    
    free(path_copy);
    
    if (parent_result != 0) {
        return -1;
    }
    
    #ifdef _WIN32
        int result = mkdir(path);
    #else
        int result = mkdir(path, 0700);
    #endif
    
    if (result != 0) {
        fprintf(stderr, "Failed to create directory %s: %s\n", 
                path, strerror(errno));
        return -1;
    }
    
    return 0;
}

// TODO(01.27.2025|jared): Eventually package this in it's own library for use in files and paths
// @paths_str should take the format: "[<output>, \"<dir>/<filename>\", \"<dir>/<dir>/<filename>\", ...]" Example: "[stderr, \".log/log.spyglass\"]" 
static char** s_parse_paths_string(const char* paths_str, int* count) 
{
    size_t len = strlen(paths_str);
    if (len < 2 || paths_str[0] != '[' || paths_str[len-1] != ']') 
    {
        *count = 0;
        return NULL;
    }

    char** result = malloc((len / 2 + 1) * sizeof(char*));
    *count = 0;

    // Skip first '[' and stop at last ']'
    char* start = (char*)paths_str + 1;
    char* end = (char*)paths_str + len;

    char current_string[256];
    int str_index = 0;

    while (start < end) 
    {
        if (str_index == 0) 
        {
            memset(current_string, 0, sizeof(current_string));
        }

        if (*start == ',' || *start == ']') 
        {
            if (str_index > 0) 
            {
                result[*count] = string_dup(current_string);
                (*count)++;
                str_index = 0;
            }
            start++;
            continue;
        }

        // Skip whitespace outside quotes
        if (*start == ' ') 
        {
            start++;
            continue;
        }

        // Skip backslashes
        if (*start == '\\') 
        {
            start++;
            continue;
        }

        // Skip Quotes
        if (*start == '"') 
        {
            start++;
            continue;
        }

        if (*start != ' ') 
        {
            current_string[str_index++] = *start;
        }

        start++;
    }

    return result;
}

// TODO(01.27.2025|jared): Eventually package this in it's own library for use in files and paths
static char* s_remove_filename_from_path(const char* path)
{
    if (path == NULL) return NULL;

    // Find the last occurrence of '/'
    const char* last_slash = NULL;
    const char* current = path;
    while (*current != '\0') {
        if (*current == '/') {
            last_slash = current;
        }
        current++;
    }
    
    if (last_slash == NULL) {
        return string_dup(".");
    }
    
    size_t dir_len = last_slash - path;
    
    char* dir_path = malloc(dir_len + 1);
    if (dir_path == NULL) {
        return NULL; 
    }
    
    size_t i;
    for (i = 0; i < dir_len; i++) {
        dir_path[i] = path[i];
    }
    dir_path[i] = '\0'; 
    
    // If directory path is empty, return root directory
    if (dir_len == 0) {
        free(dir_path);
        return string_dup("/");
    }
    
    return dir_path;
}

static void s_parse_output_list(void) 
{
    int count;
    char** parsed_strings = s_parse_paths_string(SPYGLASS_AIM, &count);

    for (int i = 0; i < count; i++)
    {
        if (strncmp(parsed_strings[i], "stderr", 6) == 0) 
        {
            s_log_files[s_output_count] = stderr;
            s_is_std[s_output_count++] = 1;
            continue;
        }
        if (strncmp(parsed_strings[i], "stdout", 6) == 0) 
        {
            s_log_files[s_output_count] = stdout;
            s_is_std[s_output_count++] = 1;
            continue;
        }
        s_ensure_directory(s_remove_filename_from_path(parsed_strings[i]));
        s_log_files[s_output_count] = fopen(parsed_strings[i], "a");
        s_is_std[s_output_count++] = 0;
    }

    for (int i = 0; i < count; i++) {
        free(parsed_strings[i]);
    }
    free(parsed_strings);
}

static void s_init_outputs(void) 
{
    s_parse_output_list();
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

    if (!s_initialized) 
    {
        s_init_outputs();
        s_initialized = 1;
    }

    for (size_t i = 0; i < s_output_count; i++) 
    {
        if (!s_log_files[i]) continue;
        
        fprintf(s_log_files[i], "%s%s", s_is_std[i] ? color : "", prefix_buffer);
        va_list args;
        va_start(args, format);
        vfprintf(s_log_files[i], format, args);
        va_end(args);
        fprintf(s_log_files[i], "%s\n", s_is_std[i] ? reset_color : "");
    
        if (!s_is_std[i]) fflush(s_log_files[i]);
    }
}

void s_log_cleanup(void) {
    for(size_t i = 0; i < s_output_count; i++)
    {
        if (!s_log_files[i]) continue;

        fclose(s_log_files[i]);
        s_log_files[i] = NULL;
    }
}