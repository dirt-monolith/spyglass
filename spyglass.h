#ifndef SPYGLASS_H
#define SPYGLASS_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifndef SPYGLASS_AIM
    #define SPYGLASS_AIM ".log"
#endif

#define SPYGLASS_LENS_TINT_MAYDAY  "\x1b[31m"
#define SPYGLASS_LENS_TINT_SIGHT   "\x1b[33m"
#define SPYGLASS_LENS_TINT_MARK    "\x1b[34m"
#define SPYGLASS_LENS_TINT_DEFAULT "\x1b[0m"

#define SPYGLASS_MAYDAY (1 << 0)
#define SPYGLASS_SIGHT  (1 << 1)
#define SPYGLASS_MARK   (1 << 2)

#define SPYGLASS_LENS_COLOR      (1 << 0)
#define SPYGLASS_LENS_TIME       (1 << 1)
#define SPYGLASS_LENS_FILE       (1 << 2)
#define SPYGLASS_LENS_FUNC       (1 << 3)
#define SPYGLASS_LENS_LINE       (1 << 4)
#define SPYGLASS_LENS_AIM        (1 << 5)
#define SPYGLASS_LENS_AIM_STDERR (1 << 6)
#define SPYGLASS_LENS_AIM_STDOUT (1 << 7)

#ifndef SPYGLASS_CONFIG_LENS
    #define SPYGLASS_CONFIG_LENS (SPYGLASS_LENS_TIME | SPYGLASS_LENS_FILE | SPYGLASS_LENS_FUNC | SPYGLASS_LENS_LINE | SPYGLASS_LENS_AIM | SPYGLASS_LENS_AIM_STDERR)
#endif

#ifndef SPYGLASS_CONFIG_OBSERVATIONS
    #define SPYGLASS_CONFIG_OBSERVATIONS (SPYGLASS_MAYDAY)
#endif

#if (SPYGLASS_CONFIG_OBSERVATIONS & SPYGLASS_MAYDAY)
    #define MAYDAY(fmt, ...) spyglass_log(SpyglassMayday, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else 
    #define MAYDAY(fmt, ...) ((void)0)
#endif

#if (SPYGLASS_CONFIG_OBSERVATIONS & SPYGLASS_SIGHT)
    #define SIGHTING(fmt, ...) spyglass_log(SpyglassSighting, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
    #define SIGHTING(fmt, ...) ((void)0)
#endif

#if (SPYGLASS_CONFIG_OBSERVATIONS & SPYGLASS_MARK)
    #define MARK(fmt, ...) spyglass_log(SpyglassMark, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#else
    #define MARK(fmt, ...) ((void)0)
#endif

typedef enum 
{
    SpyglassMayday   = SPYGLASS_MAYDAY,
    SpyglassSighting = SPYGLASS_SIGHT,
    SpyglassMark     = SPYGLASS_MARK
} spyglass_observation;

void spyglass_log(spyglass_observation level, const char* file, const char* func, int line, const char* format, ...);

#endif