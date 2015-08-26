#ifndef DEVICE_SPECS_H
#define DEVICE_SPECS_H

#include <kernel.h>

#ifndef STM32F4DISCOVERY
#error "Wrong device"
#endif

/********* Interrupts *********/
/**
 * @brief Number of interrupts.
 */
#define DEVICE_INT_COUNT (82+16)

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

/********** Kernel stack ***********/
/**
 * @brief Kernel stack start address. Must be aligned with 4
 * Stack grows down, so stack start address must be greater than stack end address.
 */
extern const void* device_kernelStackStart;

/**
 * @brief Kernel stack end address. Must be aligned with 32 for MPU stack overflow detection.
 * Stack grows down, so stack start address must be greater than stack end address.
 */
extern const void* device_kernelStackEnd;

/**
 * @brief Kernel stack size in bytes.
 */
extern const size_t device_kernelStackSize;

/********** Kernel heap **********/
/**
 * @brief Kernel heap start address.
 */
extern const void* device_kernelHeapStart;

/**
 * @brief Kernel heap start address.
 */
extern const void* device_kernelHeapEnd;

/**
 * @brief Kernel heap size in bytes.
 */
extern const size_t device_kernelHeapSize;

/********** Application memory ***********/
/**
 * @brief Application memory start address.
 */
extern const void* device_appMemoryStart;

/**
 * @brief  Application memory end address.
 */
extern const void* device_appMemoryEnd;

/**
 * @brief  Application memory size in bytes.
 */
extern const size_t device_appMemorySize;

#endif // DEVICE_SPECS_H

