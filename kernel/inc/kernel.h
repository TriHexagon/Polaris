/* The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Christopher Cichos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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

/********** Device specific routines **********/
/**
 * @brief Device initialization routine (implemented by driver).
 * Will be called before initialization of basic kernel modules in kernel_start().
 * @return Returns an error code if an error occured while initialization.
 * Otherwise ERROR_NONE.
 */
error_t device_init(void);

/**
 * @brief Device driver initialization routine (implemented by driver).
 * Will be called after kernel basic initialization was succesfull in kernel_start().
 * @return Return an error code if a fatal error occured while driver initialization.
 * Otherwise ERROR_NONE.
 */
error_t device_initDrivers(void);

/********** Memory map ***********/
/**
 * @brief Discribes a memory section (RAM).
 */
typedef struct Device_MemorySection
{
	void* start;		/**< Start address of memory section */
	void* end;			/**< End address of memory section (start+size) */
	size_t size;		/**< Size of memory address */
	bool isInternal;	/**< Is memory internal (true) or external (false) */
} Device_MemorySection;

/**
 * @brief Array of the device specific memory sections (only RAMs).
 */
extern const Device_MemorySection device_memoryMap[];

/**
 * @brief Number of entries in device_memoryMap[].
 */
extern const size_t device_memoryMapEntryCount;

/********** Constants from linker script **********/
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

#ifndef RAMMODE
/**
 * @brief Address of .data section source.
 */
 extern const size_t _dataSourceStart;
#endif

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

#endif // KERNEL_H
