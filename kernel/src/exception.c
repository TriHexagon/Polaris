#include <exception.h>
#include <interrupt.h>

#ifndef NOMPU
#include <mpu.h>
#include <device_specs.h>
#endif

#ifndef NOFPU
#include <ARMCM4_FP.h>
#else
#include <ARMCM4.h>
#endif

//exception handlers
void excpt_NMI(void);
void excpt_HardFault(void);
void excpt_MemManageFault(void);
void excpt_BusFault(void);
void excpt_UsageFault(void);

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

	/********** install exception handlers **********/
	//install NMI handler
	if ( int_install(INT_NMI, 0, 0, &excpt_NMI) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_INSTALL_FAILED);

	//install HardFault handler
	if ( int_install(INT_HARDFAULT, 0, 0, &excpt_HardFault) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_INSTALL_FAILED);

	//install MemManageFault handler
	if ( int_install(INT_MMUFAULT, 0, 0, &excpt_MemManageFault) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_INSTALL_FAILED);

	//install BusFault handler
	if ( int_install(INT_BUSFAULT, 0, 0, &excpt_BusFault) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_INSTALL_FAILED);

	//install UsageFault handler
	if ( int_install(INT_USAGEFAULT, 0, 0, &excpt_UsageFault) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_INSTALL_FAILED);

	/********** configurate MPU for kernel stack overflow detection **********/
#ifndef NOMPU
	//enable region 0, settings: baseAddress=kernel stack end, size = 32 bytes, no access
	MPU_Region region = { device_kernelStackEnd, 5, MPU_ACCESS_NO, MPU_ACCESS_NO, false };
	mpu_enableRegion(0, &region);
#endif
}

void excpt_deinit(void)
{
	/********** deactivate MPU region for kernel stack overflow detection **********/
#ifndef NOMPU
	mpu_disableRegion(0);
#endif

	/********** deinstall exception handlers **********/
	//deinstall NMI handler
	if ( int_deinstall(INT_NMI, 0) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_DEINSTALL_FAILED);

	//deinstall HardFault handler
	if ( int_deinstall(INT_HARDFAULT, 0) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_DEINSTALL_FAILED);

	//deinstall MMU handler
	if ( int_deinstall(INT_MMUFAULT, 0) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_DEINSTALL_FAILED);

	//deinstall BusFault handler
	if ( int_deinstall(INT_BUSFAULT, 0) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_DEINSTALL_FAILED);

	//deinstall UsageFault handler
	if ( int_deinstall(INT_USAGEFAULT, 0) != ERROR_NONE )
		kernel_panic(moduleName, ERROR_EXCPT_HANDLER_DEINSTALL_FAILED);
}

/********** fault handlers begin **********/
void excpt_NMI(void)
{
	debug_printf("NMI exception\n");
	kernel_panic(moduleName, ERROR_EXCPT_MNI);
}

void excpt_HardFault(void)
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

void excpt_MemManageFault(void)
{
	uint32_t MMFSR = SCB->CFSR >> SCB_CFSR_MEMFAULTSR_Pos;
	const char* str;

	//kernel stack overflow detection: check access voilation at the end of kernel stack
	if ( (MMFSR & (1<<7)) && (SCB->MMFAR >= (uint32_t)device_kernelStackEnd && SCB->MMFAR <= ((uint32_t)device_kernelStackEnd + 32)) ) //MMARVALID
	{
		debug_printf("Kernel stack overflow detection: memory access near kernel stack end detected.\nMMFAR=%x, %u bytes before kernel stack end.", SCB->MMFAR, SCB->MMFAR - (uint32_t)device_kernelStackEnd);
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

void excpt_BusFault(void)
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

void excpt_UsageFault(void)
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
