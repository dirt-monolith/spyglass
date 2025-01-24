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


typedef struct 
{
    const char* color;
    const char* reset_color;
    const char* level_str;
} s_log_config_level;

static FILE* s_log_file = NULL;

static int s_log_format_prefix(char* buffer, size_t buffer_size, const s_log_config_level* level_cfg, const char* file, const char* func, int line);
static int s_log_add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line);
static void s_log_init_file();
static int s_log_ensure_log_directory();

#endif