/**
 * @file kernel.h
 *
 * @brief Standard kernel header. Defines standard datatypes and constants which could be usefull for kernel software.
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Error code start index for module defined error codes.
 */
#define ERROR_MODULE_DEFINED 64

/**
 * @brief Null pointer constant.
 */
#define NULL ((void*)0)

/**
 * @brief Error code type.
 */
typedef unsigned error_t;

/**
 * @brief Size data type.
 */
typedef uint32_t size_t;

/**
 * @brief General purpose error codes.
 */
typedef enum ERROR
{
	ERROR_NONE = 0, ERROR_INVALID_ARGUMENT, ERROR_INVALID_INDEX, ERROR_INVALID_ADDRESS, ERROR_OUT_OF_RANGE
} ERROR;

/**
 * @brief Kernel panic handler.
 */
void kernel_panic(const char* moduleName, error_t errorCode);

#include <debug.h>

#endif // KERNEL_H

