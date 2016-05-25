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

#include <kernel.h>
#include <interrupt.h>
#include <exception.h>
#include <debug.h>
#include <mpu.h>
#include <fpu.h>
#include <heap.h>
#include <dev.h>
#include <drivers/drivers.h>
#include <device.h>

/**
 * @brief Kernel start routine.
 */
void kernel_start(void)
{
	/********** disable all interrupts **********/
	__disable_fault_irq();
	__disable_irq();

#ifndef RAMMODE
	/********** copy .data section to ram *********/
	uint32_t *src = &_dataSourceStart, *dst = &_dataStart;
	while (dst < &_dataEnd)
	{
		*dst = *src;
		src++;
		dst++;
	}

	/********** set .bss section to zero **********/
	dst = &_bssStart;
	while (dst < &_bssEnd)
	{
		*dst = 0;
		dst++;
	}
#endif

	/********** initialize system control registers (see 4.3 in Cortex-M4 Devices Generic Guide) **********/
	uint32_t tmp;

	//auxiliary control register
	tmp = SCnSCB->ACTLR;
	tmp &= ~SCnSCB_ACTLR_DISFOLD_Msk; //enable IT folding
#ifdef DEBUG
	tmp |= SCnSCB_ACTLR_DISDEFWBUF_Msk; //any store to memory must complete before next instruction (decreases performance)
#else
	tmp &= ~SCnSCB_ACTLR_DISDEFWBUF_Msk; //next intruction can be executed while store to memory
#endif
	tmp |= SCnSCB_ACTLR_DISMCYCINT_Msk; //disable interruption of load multiple and store multiple instructions (increase interrupt latency, but this is no RTOS)
	SCnSCB->ACTLR = tmp;

	//control register
	tmp = SCB->SCR;
	tmp &= ~SCB_SCR_SEVONPEND_Msk; //only enabled that interrupts/events can wakeup processor
	tmp &= ~SCB_SCR_SLEEPDEEP_Msk; //no deep sleep in low power mode
	tmp &= ~SCB_SCR_SLEEPONEXIT_Msk; //no sleep when return to thread mode
	SCB->SCR = tmp;

	/********** initialize kernel base modules **********/
	if (device_init() != ERROR_NONE)
		for (;;) {}

#if __MPU_PRESENT && !defined NOMPU
	mpu_init();
#endif

	int_init();
	excpt_init();

#if __FPU_PRESENT && !defined NOFPU
	fpu_init();
#endif

	heap_init();

	/*********** initialize advanced kernel modules **********/
	dev_init();

	/********** initialize driver modules **********/
	if (device_initDrivers() != ERROR_NONE)
		for (;;) {}

	debug_printf("Kernel is ready.\n");
	led_set(0, LED_ENABLE);

	for (;;) {}
}

/**
 * @brief Kernel panic routine.
 * Call it when an error occures which cannot be fixed and ignored.
 * @param moduleName Module name of the caller.
 * @param errorCode Error code.
 */
void kernel_panic(const char* moduleName, error_t errorCode)
{
	if (led_isInit())
		if (led_getCount() > 0)
			led_set(0, LED_ENABLE);

	debug_printf("KERNEL PANIC asserted by %s module, error code %i\n", moduleName, errorCode);

	for (;;) {}
}
