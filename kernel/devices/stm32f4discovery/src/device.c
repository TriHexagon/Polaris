#include <device.h>
#include <drivers/drivers.h>

#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

/********** Memory map ***********/
const Device_MemorySection device_memoryMap[] =
{
     { (void*)0x20000000, (void*)0x2001FFFF, 1024*128, true },	//SRAM1 (112KiB) & SRAM2 (16KiB)
     { (void*)0x10000000, (void*)0x1000FFFF, 1024*64, true} 	//CCM	(64KB)
 };

const size_t device_memoryMapEntryCount = 2;

/********** Device initialization routines **********/
void SystemInit(void); //CMSIS initialization

error_t device_init(void)
{
    SystemInit();

    return ERROR_NONE;
}

error_t device_initDrivers(void)
{
    led_init();
    uart_init();

    return ERROR_NONE;
}
