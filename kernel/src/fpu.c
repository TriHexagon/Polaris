#ifndef NOFPU

#include <fpu.h>

#include <ARMCM4_FP.h>

void fpu_init(void)
{
	SCB->CPACR |= (3<<10*2) | (3<<11*2); //full access CP10 CP11
	__DSB(); //sync store
	__ISB(); //reset pipeline
}

void fpu_deinit(void)
{
	SCB->CPACR &= ~((3<<10*2) | (3<<11*2));
	__DSB(); //sync store
	__ISB(); //reset pipeline
}

#endif
