#include <kernel.h>
#include <interrupt.h>
#include <exception.h>
#include <debug.h>
#include <mpu.h>
#include <fpu.h>
#include <heap.h>
#include <drivers/drivers.h>

#ifndef NOFPU
#include <ARMCM4_FP.h>
#else
#include <ARMCM4.h>
#endif

/**
 * @brief Kernel start routine.
 */
void kernel_start(void)
{
	/********** disable all interrupts **********/
	__disable_fault_irq();
	__disable_irq();

	/********** initialize system control registers (see 4.3 in Cortex-M4 Devices Generic Guide) **********/
	uint32_t tmp;

	//auxiliary control register
	tmp = SCnSCB->ACTLR;
	tmp &= ~SCnSCB_ACTLR_DISFOLD_Msk; //enable IT folding
#ifdef DEBUG
	tmp |= SCnSCB_ACTLR_DISDEFWBUF_Msk; //Any store to memory must complete before next instruction (decreases performance)
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

#ifndef NOMPU
	mpu_init();
#endif

	int_init();
	excpt_init();

#ifndef NOFPU
	fpu_init();
#endif

	heap_init();

	/********** initialize driver modules **********/

	led_init();
	uart_init();

	debug_printf("Kernel is ready.\n");

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
