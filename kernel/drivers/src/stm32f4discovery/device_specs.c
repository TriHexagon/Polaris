#include <device_specs.h>

#ifndef STM32F4DISCOVERY
#error "Wrong device"
#endif

/********** Memory map ***********/
const Device_MemorySection device_memoryMap[] =
{ { (void*)0x20000000, (void*)0x2001BFFF, 1024*112, true },	//SRAM1 (112KB)
{ (void*)0x2001C000, (void*)0x2001FFFF, 1024*16, true },	//SRAM2 (16KB)
{ (void*)0x10000000, (void*)0x1000FFFF, 1024*64, true} };	//CCM	(64KB)

const size_t device_memoryMapEntryCount = 3;

/********** Kernel stack ***********/
const void* device_kernelStackEnd	= (void*)0x10000000; //CCM begin
const size_t device_kernelStackSize	= 1024*5; //5KB
const void* device_kernelStackStart	= (void*)(0x10000000 + 1024*5 -4); //CCM begin + 5KB (aligned with 4)

/********** Kernel heap **********/
const void* device_kernelHeapStart	= (void*)(0x10000000 + 1024*5);	//CCM begin + 5KB
const size_t device_kernelHeapSize	= 1024*59; //59KB
const void* device_kernelHeapEnd	= (void*)0x1000FFFF; //CCM end

/********** Application memory ***********/
const void* device_appMemoryStart	= (void*)(0x20000000 + 1024*12); //SRAM1 begin + 12KB
const size_t device_appMemorySize	= 1024*100; //100KB
const void* device_appMemoryEnd		= (void*)0x2001BFFF; //SRAM1 end
