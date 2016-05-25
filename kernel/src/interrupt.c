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

/* Kernel interrupt (int) module.
 *
 * Manages interrupt vector table and the access of it.
 */

#include <interrupt.h>

#include <device.h>

const static char moduleName[] = "int";

/**
 * @brief TBLBASE bit mask of VTOR register.
 */
#define TBLBASE (1<<29)

/* Deklaration of basic interrupt handlers */
void handler_default(void);
void handler_reset(void);
void __attribute__((weak)) handler_nmi(void);
void __attribute__((weak)) handler_hardfault(void);
void __attribute__((weak)) handler_mmufault(void);
void __attribute__((weak)) handler_busfault(void);
void __attribute__((weak)) handler_usagefault(void);
void __attribute__((weak)) handler_svcall(void);
void __attribute__((weak)) handler_pendsv(void);
void __attribute__((weak)) handler_systick(void);

/**
 * @brief Saves the old interrupt vector table address which will be reloaded at kernel shutdown.
 */
static uint32_t oldVectorAddress = 0;

typedef void (*IntHandler)(void);

/**
 * @brief The interrupt vector table.
 */
__attribute__ ((section (".ivector"))) const IntHandler ivectorTable[16] =
{
	(void*)&_stackStart,	//stack start pointer
	handler_reset,			//Reset handler
	handler_nmi,			//NMI handler
	handler_hardfault,		//HardFault handler
	handler_mmufault,		//MMU handler
	handler_busfault,		//BusFault handler
	handler_usagefault,		//UsageFault handler
	0,						//reserved
	0,						//reserved
	0,						//reserved
	0,						//reserved
	handler_svcall,			//SVCall handler
	handler_default,		//DebugMonitor handler
	0,						//reserved
	handler_pendsv,			//PendSV handler
	handler_systick			//SysTick handler
};

#pragma weak handler_nmi = handler_default
#pragma weak handler_hardfault = handler_default
#pragma weak handler_mmufault = handler_default
#pragma weak handler_busfault = handler_default
#pragma weak handler_usagefault = handler_default
#pragma weak handler_svcall = handler_default
#pragma weak handler_pendsv = handler_default
#pragma weak handler_systick = handler_default

void handler_default(void)
{
	kernel_panic(moduleName, ERROR_INT_NO_HANDLER);
}

void int_init(void)
{
	uint32_t tmp;

	/********** disable interrupts **********/
	__disable_fault_irq();
	__disable_irq();

	/********** initialize system control register (interrupt specific register) **********/

	//VTOR register will be initialized later

	//application interrupt and reset control register
	tmp = SCB->AIRCR;
	tmp |= SCB_AIRCR_PRIGROUP_Msk; //no group priorities (0b111 mask)
	SCB->AIRCR = tmp;

	//disable all fault interrupts which can be disabled
	tmp = SCB->SHCSR;
	tmp &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);  //usage-,bus-, memfault
	SCB->SHCSR = tmp;

	/********** disable all IRQs **********/
	for (size_t reg = 0; reg <= 7; reg++)
		NVIC->ICER[reg] = 0;

	/********** save old interrupt vector table address **********/
	oldVectorAddress = SCB->VTOR;

	/********** sets new interrupt vector table (set TBLBASE bit because interrupt vector table is in SRAM) **********/
	SCB->VTOR = ( ( (uint32_t)ivectorTable) | TBLBASE ) & SCB_VTOR_TBLOFF_Msk;

	/********** enable interrupts **********/
	__enable_irq();
	__enable_fault_irq();
}

void int_deinit(void)
{
	//reload old interrupt vector table address
	SCB->VTOR = oldVectorAddress;
}

void int_enable(int32_t irqNum, uint8_t priority)
{
	//set correct priority value
	priority = (priority<<(8 - __NVIC_PRIO_BITS));

	//basic fault handler interrupts
	if ( irqNum < 0 )
	{
		//priority for reset, nmi and harfault interrupt is fixed
		//enable interrupt if it can be disabled
		uint32_t tmp = SCB->SHCSR;
		switch (irqNum)
		{
		case INT_MMUFAULT:
			tmp |= SCB_SHCSR_MEMFAULTENA_Msk;
			SCB->SHP[4] = priority;
			break;

		case INT_BUSFAULT:
			tmp |= SCB_SHCSR_BUSFAULTENA_Msk;
			SCB->SHP[5] = priority;
			break;

		case INT_USAGEFAULT:
			tmp |= SCB_SHCSR_USGFAULTENA_Msk;
			SCB->SHP[6] = priority;
			break;

		case INT_SVCALL:
			//interrupt is always enabled
			SCB->SHP[11] = priority;
			break;

		case INT_PENDSV:
			//interrupt is always enabled
			SCB->SHP[14] = priority;
			break;

		case INT_SYSTICK:
			//interrupt is always enabled
			SCB->SHP[15] = priority;
			break;
		}

		SCB->SHCSR = tmp;
	}
	//IRQ interrupt
	else if (irqNum <= DEVICE_INT_COUNT)
	{
		NVIC->IP[irqNum] = priority;
		NVIC_EnableIRQ(irqNum);
	}
}

void int_disable(int32_t irqNum)
{
	//IRQ interrupt
	if ( irqNum >= 0 && irqNum <= DEVICE_INT_COUNT )
	{
		NVIC_DisableIRQ(irqNum);
		return;
	}

	//basic fault handler interrupts (only MMU-, Bus- and Usagefault can be disabled)
	uint32_t tmp = SCB->SHCSR;
	switch (irqNum)
	{
	case INT_MMUFAULT:
		tmp &= ~SCB_SHCSR_MEMFAULTENA_Msk;
		break;

	case INT_BUSFAULT:
		tmp &= ~SCB_SHCSR_BUSFAULTENA_Msk;
		break;

	case INT_USAGEFAULT:
		tmp &= ~SCB_SHCSR_USGFAULTENA_Msk;
		break;
	}
	SCB->SHCSR = tmp;
}
