#ifndef SPYGLASS_INTERNAL_H
#define SPYGLASS_INTERNAL_H

#include "spyglass.h"
#include <stddef.h>

typedef struct 
{
    const char* color;
    const char* reset_color;
    const char* level_str;
} s_log_config_level;

static int s_log_format_prefix(char* buffer, size_t buffer_size, const s_log_config_level* level_cfg, const char* file, const char* func, int line);
static int s_log_add_source_location(char* buffer, size_t buffer_size, const char* file, const char* func, int line);

#endif