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
 * @file heap.h
 *
 * @brief Heap module. Provides a heap memory management. Kernel software can request dynamic memory allocation here.
 */

#ifndef HEAP_H
#define HEAP_H

#include <kernel.h>

/**
 * @brief Initialize the heap memory.
 */
void heap_init(void);

/**
 * @brief Allocate memory with specified size from heap.
 * @param size The size of the needed memory.
 * @return A valid address to the memory block.
 * Otherwise NULL if no memory or memory with the specified size is aviable.
 */
void* heap_alloc(size_t size);

/**
 * @brief Free the memory which was allocated from heap.
 * @param mem The address from the memory. heap_free will do nothing if mem is NULL.
 */
void heap_free(void* mem);

/**
 * @brief Returns stats of heap.
 * @param outHeapSize Returns the heap size if outHeapSize isn't NULL.
 * @param outAllocMem Returns the size of allocated memory if outAllocMem isn't NULL.
 * @param outFreeMem Returns the size of free memory if outFreeMem isn't NULL.
 */
void heap_getStats(size_t* outHeapSize, size_t* outAllocMem, size_t* outFreeMem);

#endif // HEAP_H

