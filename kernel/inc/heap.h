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

