#ifndef SPYGLASS_H
#define SPYGLASS_H

#include <stdio.h>

typedef enum {
    SPYGLASS_LOG_ERROR,
    SPYGLASS_LOG_WARNING,
    SPYGLASS_LOG_INFO
} spyglass_log_level;

void spyglass_log_print(spyglass_log_level level, const char* message);

#endif