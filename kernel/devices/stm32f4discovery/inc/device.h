#ifndef DEVICE_SPECS_H
#define DEVICE_SPECS_H

#include <kernel.h>

#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

#define STM32F40_41xxx
#include <stm32f4xx.h>

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

/********* Interrupts *********/
/**
 * @brief Number of interrupts.
 */
#define DEVICE_INT_COUNT (82+16)

#endif // DEVICE_SPECS_H
