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
 * @file interrupt.h
 *
 * @brief Interrupt module. Initializes interrupt vector table and controlles the NVIC.
 * Also provides functions of interrupt handler installation/deinstallation for kernel software (especially for drivers and exception module).
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <kernel.h>

/**
 * @brief The exception number constant of the first IRQ.
 */
#define INT_IRQ_EXCPT_START 16

/**
 * @brief Returns the exception number of the IRQ number.
 */
#define INT_IRQ_EXCPT_NUM(irq) (INT_IRQ_EXCPT_START+irq)

/**
 * @brief Returns the IRQ number of exception number.
 */
#define INT_EXCPT_IRQ_NUM(excp) (excp-INT_IRQ_EXCPT_START)

/**
 * @brief The number of interrupts.
 */
#define INT_COUNT 255


#define INT_IRQ_COUNT (INT_COUNT-INT_IRQ_EXCPT_START)

/**
 * @brief The first IRQ number.
 */
#define INT_IRQ_FIRST 0

/**
 * @brief The last IRQ number.
 */
#define INT_IRQ_LAST (INT_IRQ_COUNT-1)

#define ERROR_INT_ALREADY_IN_USE (ERROR_MODULE_DEFINED)
#define ERROR_INT_NOT_USED_YET (ERROR_MODULE_DEFINED+1)

#define ERROR_INT_NO_HANDLER (ERROR_MODULE_DEFINED+2)

#define INT_PRIORITY_LOWEST 15

#define INT_SOFTWARE_HIGH_PRIORITY (INT_PRIORITY_LOWEST-2)
#define INT_SOFTWARE_AVERAGE_PRIORITY (INT_PRIORITY_LOWEST-1)
#define INT_SOFTWARE_LOW_PRIORITY (INT_PRIORITY_LOWEST)

/**
 * @brief System interrupt types.
 */
typedef enum INT_TYPE
{
	INT_RESET		= -15,	/**< The reset interrupt. */
	INT_NMI			= -14,	/**< The NMI interrupt. */
	INT_HARDFAULT	= -13,	/**< The hard fault interrupt. */
	INT_MMUFAULT	= -12,	/**< The memory managment fault interrupt. */
	INT_BUSFAULT	= -11,	/**< The bus fault interrupt. */
	INT_USAGEFAULT	= -10,	/**< The usage fault interrupt. */
	INT_SVCALL		= -5,	/**< The svcall interrupt. */
	INT_PENDSV		= -2,	/**< The pendsv interrupt. */
	INT_SYSTICK		= -1,	/**< The systick interrupt. */
	INT_IRQ			= 0,	/**< An IRQ interrupt. */
} INT_TYPE;

/**
 * @brief Initializes interrupt (int) driver and saves address of old interrupt vector table.
 *
 * MUST be called before module usage!
 */
void int_init(void);

/**
 * @brief Reloads old interrupt vector table address.
 *
 * After call driver is in uninitialized state. So module usage is permited until int_init call.
 */
void int_deinit(void);

/**
 * @brief Enables an interrupt.
 * @param The irq number of the interrupt. If the irq number is invalid, this function does nothing.
 * @param The priority of the interrupt.
 */
void int_enable(int32_t irqNum, uint8_t priority);

/**
 * @brief Disables an interrupt.
 * @param The irq number of the interrupt. If the irq number is invalid, this function does nothing.
 */
void int_disable(int32_t irqNum);

#endif // INTERRUPT_H

