/* Kernel interrupt (int) module.
 *
 * Manages interrupt vector table and the access of it.
 */

#include <interrupt.h>
#include <device_specs.h>

#ifndef NOFPU
#include <ARMCM4_FP.h>
#else
#include <ARMCM4.h>
#endif

/**
 * @brief Stack start address from linker script.
 */
extern const void* _stackStart;

const static char moduleName[] = "int";

/**
 * @brief TBLBASE bit mask of VTOR register.
 */
#define TBLBASE (1<<29)

/**
 * @brief Saves the old interrupt vector table address which will be reloaded at kernel shutdown.
 */
static uint32_t oldVectorAddress = 0;

/**
 * @brief The interrupt vector table.
 */
IntHandler vecTable[DEVICE_INT_COUNT] __attribute__ ((section (".vector_table")));

void int_defaultHandler(void)
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

	/********** initialize int_table with default handler **********/
	for (size_t i = 0; i < DEVICE_INT_COUNT; i++)
		vecTable[i] = &int_defaultHandler;

	/********** sets new interrupt vector table (set TBLBASE bit because interrupt vector table is in SRAM) **********/
	SCB->VTOR = ( ( (uint32_t)&vecTable -4) | TBLBASE ) & SCB_VTOR_TBLOFF_Msk;

	/********** enable interrupts **********/
	__enable_irq();
	__enable_fault_irq();
}

void int_deinit(void)
{
	//reload old interrupt vector table address
	SCB->VTOR = oldVectorAddress;
}

error_t int_install(INT_TYPE type, uint8_t irqNum, uint8_t priority, IntHandler handler)
{
	//arm cortex specific interrupts
	if (type == INT_RESET || type == INT_NMI || type == INT_HARDFAULT ||
		type == INT_MMUFAULT || type == INT_BUSFAULT || type == INT_USAGEFAULT ||
		type == INT_SVCALL || type == INT_PENDSV || type == INT_SYSTICK)
	{
		//ignore irqNum

		//check priority
		if (priority > INT_PRIORITY_LOWEST)
			return ERROR_INVALID_ARGUMENT;

		//check if entry in table is already in use
		if (vecTable[type-1] != &int_defaultHandler)
			return ERROR_INT_ALREADY_IN_USE;

		//set handler
		vecTable[type-1] = handler;

		//set priority (for arm cortex specific interrupts the priority is 0)
		//priority for reset, nmi and harfault interrupt is fixed
		//enable interrupt if it can be disabled
		if (type != INT_RESET && type != INT_NMI && type != INT_HARDFAULT)
		{
			NVIC_SetPriority(INT_EXCPT_IRQ_NUM(type), priority);
			uint32_t tmp = SCB->SHCSR;
			switch (type)
			{
				case INT_USAGEFAULT:
					tmp |= SCB_SHCSR_USGFAULTENA_Msk;
					break;
				case INT_BUSFAULT:
					tmp |= SCB_SHCSR_BUSFAULTENA_Msk;
					break;
				case INT_MMUFAULT:
					tmp |= SCB_SHCSR_MEMFAULTENA_Msk;
					break;
			}

			SCB->SHCSR = tmp;
		}

	}
	//an IRQ interrupt
	else if (type == INT_IRQ)
	{
		//check irqNumber
		if (irqNum < DEVICE_INT_COUNT)
			return ERROR_INVALID_ARGUMENT;

		//check priority
		if (priority > INT_PRIORITY_LOWEST)
			return ERROR_INVALID_ARGUMENT;

		//check if entry in table is already in use
		if (vecTable[INT_IRQ_EXCPT_NUM(irqNum)-1] != &int_defaultHandler)
			return ERROR_INT_ALREADY_IN_USE;

		//set handler
		vecTable[INT_IRQ_EXCPT_NUM(irqNum)-1] = handler;

		//set priority
		NVIC_SetPriority(irqNum, priority);

		//enable interrupt
		NVIC_EnableIRQ(irqNum);
	}
	//unknown type
	else
	{
		return ERROR_INVALID_ARGUMENT;
	}

	return ERROR_NONE;
}

error_t int_deinstall(INT_TYPE type, uint8_t irqNum)
{
	//arm cortex specific interrupts
	if (type == INT_RESET || type == INT_NMI || type == INT_HARDFAULT ||
		type == INT_MMUFAULT || type == INT_BUSFAULT || type == INT_USAGEFAULT ||
		type == INT_SVCALL || type == INT_PENDSV || type == INT_SYSTICK)
	{
		//ignore irqNum

		//check if entry in table is in use
		if (vecTable[type-1] == &int_defaultHandler)
			return ERROR_INT_NOT_USED_YET;

		//disable interrupt if it can be disabled
		uint32_t tmp = SCB->SHCSR;
		switch (type)
		{
			case INT_USAGEFAULT:
				tmp &= ~SCB_SHCSR_USGFAULTENA_Msk;
				break;
			case INT_BUSFAULT:
				tmp &= ~SCB_SHCSR_BUSFAULTENA_Msk;
				break;
			case INT_MMUFAULT:
				tmp &= ~SCB_SHCSR_MEMFAULTENA_Msk;
				break;
		}
		SCB->SHCSR = tmp;

		//reset entry to default handler
		vecTable[type-1] = &int_defaultHandler;
	}
	//an IRQ interrupt
	else if (type == INT_IRQ)
	{
		//check irqNum
		if (irqNum > DEVICE_INT_COUNT)
			return ERROR_INVALID_ARGUMENT;

		//check if entry in table is in use
		if (vecTable[type-1] == &int_defaultHandler)
			return ERROR_INT_NOT_USED_YET;

		//disable interrupt
		NVIC_DisableIRQ(irqNum);

		//reset entry to default handler
		vecTable[INT_IRQ_EXCPT_NUM(irqNum)-1] = &int_defaultHandler;
	}
	else
	{
		return ERROR_INVALID_ARGUMENT;
	}

	return ERROR_NONE;
}
