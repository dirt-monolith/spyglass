#include "spyglass.h"
#include <stdarg.h>

void spyglass_log_print(spyglass_log_level level, const char* format, ...) 
{
    va_list args;
    va_start(args, format);

    switch(level)
    {
        case SPYGLASS_LOG_ERROR:
            fprintf(stderr, "ERROR: ");
            vfprintf(stderr, format, args);
            break;
        case SPYGLASS_LOG_WARNING:
            fprintf(stderr, "WARNING: ");
            vfprintf(stderr, format, args);
            break;
        case SPYGLASS_LOG_INFO:
            fprintf(stderr, "INFO: ");
            vfprintf(stderr, format, args);
            break;
    }
    fprintf(stderr, "\n");

    va_end(args);
}