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

//Constants from linker script
/**
 * @brief Address of .text section start.
 */
extern const size_t _textStart;

/**
 * @brief Address of .text section end.
 */
extern const size_t _textEnd;

/**
 * @brief Size of .text section.
 */
extern const size_t _textSize;

/**
 * @brief Address of .data section start.
 */
extern const size_t _dataStart;

/**
 * @brief Address of .data section end.
 */
extern const size_t _dataEnd;

/**
 * @brief Size of .data section.
 */
extern const size_t _dataSize;

/**
 * @brief Address of .bss start.
 */
extern const size_t _bssStart;

/**
 * @brief Address of .bss end.
 */
extern const size_t _bssEnd;

/**
 * @brief Size of .bss section.
 */
extern const size_t _bssSize;

/**
 * @brief Address of stack start.
 */
extern const size_t _stackStart;

/**
 * @brief Address of stack end.
 */
extern const size_t _stackEnd;

/**
 * @brief Address of stack end.
 */
extern const size_t _stackSize;

/**
 * @brief Address of heap.
 */
extern const size_t _heapStart;

/**
 * @brief Size of heap.
 */
extern const size_t _heapSize;

/**
 * @brief Address of userspace start.
 */
extern const size_t _userspaceStart;

/**
 * @brief Address of userspace end.
 */
extern const size_t _userspaceEnd;

/**
 * @brief Size of userspace.
 */
extern const size_t _userspaceSize;

#include <debug.h>

#endif // KERNEL_H

