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

#include <exception.h>
#include <interrupt.h>
#include <device.h>

#if __MPU_PRESENT && !defined NOMPU
#include <mpu.h>
#endif

static const char moduleName[] = "excpt";

void excpt_init(void)
{
	/********** initialize system control register (exception specific registers) **********/
	uint32_t tmp;

	//configuration and control register
	tmp = SCB->CCR;
	tmp &= ~SCB_CCR_STKALIGN_Msk; //4-byte stack alignment on exception entry
	tmp &= ~SCB_CCR_BFHFNMIGN_Msk; //PendSV and SVCall handlers cannot ignore bus faults
	tmp |= SCB_CCR_DIV_0_TRP_Msk; //division by 0 cause an usage fault
	tmp |= SCB_CCR_UNALIGN_TRP_Msk; //unaligned access cause an usage fault
	SCB->CCR = tmp;

	/********** enable exception interrupts **********/
	//NMI and HardFault interrupts are always enabled

	//enable MemManageFault interrupt
	int_enable(INT_MMUFAULT, 0);

	//enable BusFault interrupt
	int_enable(INT_BUSFAULT, 0);

	//enable UsageFault interrupt
	int_enable(INT_USAGEFAULT, 0);

	/********** configurate MPU for kernel stack overflow detection **********/
#if __MPU_PRESENT && !defined NOMPU
	//enable region 0, settings: baseAddress=kernel stack end, size = 32 bytes, no access
	MPU_Region region = { &_stackEnd, 5, MPU_ACCESS_NO, MPU_ACCESS_NO, false };
	mpu_enableRegion(0, &region);
#endif
}

void excpt_deinit(void)
{
	/********** deactivate MPU region for kernel stack overflow detection **********/
#if __MPU_PRESENT && !defined NOMPU
	mpu_disableRegion(0);
#endif

	/********** disable exception interrupts **********/
	//NMI and HardFault interrupts are always enabled

	//disable MemManageFault interrupt
	int_disable(INT_MMUFAULT);

	//disable BusFault interrupt
	int_disable(INT_BUSFAULT);

	//disable UsageFault interrupt
	int_disable(INT_USAGEFAULT);
}

/********** fault handlers begin (weak linkage) **********/
void handler_nmi(void)
{
	debug_printf("NMI exception\n");
	kernel_panic(moduleName, ERROR_EXCPT_MNI);
}

void handler_hardfault(void)
{
	//TODO: make handler safe for possible stack corruption
	uint32_t HFSR = SCB->HFSR;
	const char* str;
	if (HFSR & (1<<30)) //FORCED
		str = "forced hard fault (FORCED)";
	else if (HFSR & (1<<1)) //VECTTBL
		str = "vector table read (VECTTBL)";
	else
		str = "undefined reason";

	debug_printf("HardFault exception: %s\n", str);
	kernel_panic(moduleName, ERROR_EXCPT_HARDFAULT);
}

void handler_mmufault(void)
{
	uint32_t MMFSR = SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos;
	const char* str;

	//kernel stack overflow detection: check access voilation at the end of kernel stack
	if ( (MMFSR & (1<<7)) && (SCB->MMFAR >= (uint32_t)_stackEnd && SCB->MMFAR <= ((uint32_t)&_stackEnd + 32)) ) //MMARVALID
	{
		debug_printf("Kernel stack overflow detection: memory access near kernel stack end detected.\nMMFAR=%x, %u bytes before kernel stack end.", SCB->MMFAR, SCB->MMFAR - (uint32_t)&_stackEnd);
		kernel_panic(moduleName, ERROR_EXCPT_MMUFAULT);
	}

	if (MMFSR & (1<<5)) //MLSPERR
		str = "floating-point lazy state preservation (MLSPERR)";
	else if (MMFSR & (1<<4)) //MSTKERR
		str = "access violation while stacking for an exception entry (MSTKERR)";
	else if (MMFSR & (1<<3)) //MUNSTKERR
		str = "access violation while unstacking for an exception return (MUNSTKERR)";
	else if (MMFSR & (1<<1)) //DACCVIOL
		str = "data access violation (DACCVIOL)";
	else if (MMFSR & (1<<0)) //IACCVIOL
		str = "instruction access violation (IACCVIOL)";
	else
		str = "undefined reason";

	debug_printf("MemManageFault exception: %s\n", str);
	if (MMFSR & (1<<7)) //MMARVALID
		debug_printf("MMFAR=%x\n", SCB->MMFAR);

	kernel_panic(moduleName, ERROR_EXCPT_MMUFAULT);
}

void handler_busfault(void)
{
	uint32_t BFSR = SCB->CFSR >> SCB_CFSR_BUSFAULTSR_Pos;
	const char* str;
	if (BFSR & (1<<5)) //LSPERR
		str = "floating-point lazy state preservation (LSPERR)";
	else if (BFSR & (1<<3)) //UNSTKERR
		str = "unstack for an exception return (UNSTKERR)";
	else if (BFSR & (1<<2)) //IMPRECISERR
		str = "data bus error (IMPRECISERR)";
	else if (BFSR & (1<<1)) //PRECISERR
		str = "data bus error (PRECISERR)";
	else if (BFSR & (1<<0)) //IBUSERR
		str = "instrucrion bus error (IBUSERR)";
	else
		str = "undefined reason";

	debug_printf("BusFault exception: %s\n", str);
	if (BFSR & (1<<7)) //if BFAR has valid value (BFARVALID)
		debug_printf("BFAR=%x\n", SCB->BFAR);

	kernel_panic(moduleName, ERROR_EXCPT_BUSFAULT);
}

void handler_usagefault(void)
{
	uint32_t UFSR = SCB->CFSR >> SCB_CFSR_USGFAULTSR_Pos;
	const char* str;
	if (UFSR & (1<<9)) //DIVBYZERO
		str = "division by zero (DIVBYZERO)";
	else if (UFSR & (1<<8)) //UNALIGNED
		str = "unaligned memory access (UNALIGNED)";
	else if (UFSR & (1<<3)) //NOCP
		str = "no coprocessor (NOCP)";
	else if (UFSR & (1<<2)) //INVPC
		str = "invalid PC load (INVPC)";
	else if (UFSR & (1<<1)) //INVSTATE
		str = "invalid state (INVSTATE)";
	else if (UFSR & (1<<0)) //UNDEFINSTR
		str = "undefined instruction (UNDEFINSTR)";
	else
		str = "undefined reason";

	debug_printf("UsageFault exception: %s\n", str);

	kernel_panic(moduleName, ERROR_EXCPT_USAGEFAULT);
}

/********** fault handlers end **********/
