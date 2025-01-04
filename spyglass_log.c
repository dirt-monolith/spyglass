#include "spyglass.h"

void spyglass_log_print(spyglass_log_level level, const char* message) 
{
    switch(level)
    {
        case SPYGLASS_LOG_ERROR:
            fprintf(stderr, "ERROR: %s\n", message);
            break;
        case SPYGLASS_LOG_WARNING:
            fprintf(stderr, "WARNING: %s\n", message);
            break;
        case SPYGLASS_LOG_INFO:
            fprintf(stderr, "INFO: %s\n", message);
            break;
        default:
            fprintf(stderr, "FATAL: Unsupported spyglass log level: %d\n", level);
    };
}