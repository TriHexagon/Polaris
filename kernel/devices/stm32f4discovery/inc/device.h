#ifndef DEVICE_SPECS_H
#define DEVICE_SPECS_H

#include <kernel.h>

#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

#define STM32F40_41xxx
#include <stm32f4xx.h>

/**
 * @brief Number of interrupts.
 */
#define DEVICE_INT_COUNT (82+16)

#endif // DEVICE_SPECS_H
