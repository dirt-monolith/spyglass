#ifndef SPYGLASS_INTERNAL_H
#define SPYGLASS_INTERNAL_H

#include "spyglass.h"
#include <stddef.h>

/*
* Cross-platform cleanup mechanism for file handling
* 
* This code ensures proper file cleanup across different operating systems and compilers:
* 
* Apple/macOS (__APPLE__):
* - Uses pthread for thread-safe initialization
* - Requires pthread.h for thread safety primitives
* 
* Microsoft Visual C++ (_MSC_VER):
* - Creates special CRT section for cleanup using .CRT$XCU
* - Uses MSVC-specific __declspec for cleanup registration
* 
* Other platforms (Linux, BSD, Unix):
* - Uses GCC/Clang destructor attribute
* 
* The cleanup function (spyglass_cleanup) automatically runs when:
* 1. Program exits normally
* 2. Process terminates unexpectedly
* 3. s_log_file goes out of scope
* 
* This ensures no file handles are leaked and all data is properly flushed,
* maintaining system resource integrity.
*/
#ifdef __APPLE__
    #include <pthread.h>
    static pthread_once_t s_cleanup_once = PTHREAD_ONCE_INIT;
    static void __attribute__((destructor)) s_log_cleanup(void);
#elif defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    static void __declspec(allocate(".CRT$XCU")) s_log_cleanup(void);
#else
    static void __attribute__((destructor)) s_log_cleanup(void);
#endif

#if SPYGLASS_CONFIG_LENS & SPYGLASS_LENS_AIM_STDOUT
    #define LOG_OUTPUT stdout
#else
    #define LOG_OUTPUT stderr
#endif

typedef struct 
{
    const char* color;
    const char* reset_color;
    const char* spot_str;
} s_config_spot;

static size_t s_output_count = 0;
static FILE* s_log_files[8];
static int s_is_std[8];
static int s_initialized = 0;

static int s_format_prefix(char* buffer, size_t buffer_size, const s_config_spot* cfg_spot, const char* file, const char* func, int line);
static int s_add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line);
static int s_ensure_directory(const char* dir);
static char** s_parse_paths_string(const char* macro_str, int* count);
static char* s_dirname(char* path);
static char* s_remove_filename_from_path(const char* path);
static char* string_dup(const char* str);

#endif /* ----- SPYGLASS_INTERNAL_H -----*/