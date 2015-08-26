/**
 * @file debug.h
 *
 * @brief Debug module. Provides a function to print debug messages.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <kernel.h>

#ifdef DEBUG

void debug_printf(const char* fmt, ...);

#else

#define debug_printf(fmt, ...)

#endif

#endif // DEBUG_H

