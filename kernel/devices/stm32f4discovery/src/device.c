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
