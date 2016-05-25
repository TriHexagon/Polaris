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

/* StatusLed Driver for STM32F4Discovery Device
 *
 * The device has four user leds:
 * - LD3 (PD13, orange)
 * - LD4 (PD12, green)
 * - LD5 (PD14, red)
 * - LD6 (PD15, blue)
 *
 * Needs access to following modules:
 * - RCC
 * - GPIOD
 */

#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

#include <drivers/led.h>

#define STM32F40_41xxx
#include <stm32f4xx.h>

static bool isInit = false;

bool led_isInit(void)
{
	return isInit;
}

error_t led_init(void)
{
	uint32_t tmp;

	//enable GPIOD clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	//set PD13, PD12, PD14, PD15 to output mode
	tmp = GPIOD->MODER;
	tmp &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER12 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
	tmp |= GPIO_MODER_MODER13_0 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
	GPIOD->MODER = tmp;

	//set PD13, PD12, PD14, PD15 to push-pull mode
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);

	//set PD13, PD12, PD14, PD15 to low speed mode
	GPIOD->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);

	//set PD13, PD12, PD14, PD15 to pull-up mode
	tmp = GPIOD->PUPDR;
	tmp &= ~(GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
	tmp |= GPIO_PUPDR_PUPDR13_0 | GPIO_PUPDR_PUPDR12_0 | GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR15_0;

	//set PD13, PD12, PD14, PD15 to low state (disable all LEDs)
	GPIOD->BSRRH |= (GPIO_BSRR_BS_13 | GPIO_BSRR_BS_12 | GPIO_BSRR_BS_14 | GPIO_BSRR_BS_15);

	isInit = true;

	return ERROR_NONE;
}

unsigned led_getCount(void)
{
	return 4;
}

error_t led_set(unsigned index, LED_SET set)
{
	if (index >= 4)
	{
		return ERROR_INVALID_INDEX;
	}

	if (set == LED_ENABLE)
	{
		switch (index)
		{
			case 0: //LD3 (PD13)
				GPIOD->BSRRL |= GPIO_BSRR_BS_13;
				break;
			case 1: //LD4 (PD12)
				GPIOD->BSRRL |= GPIO_BSRR_BS_12;
				break;
			case 2: //LD5 (PD14)
				GPIOD->BSRRL |= GPIO_BSRR_BS_14;
				break;
			case 3: //LD6 (PD15)
				GPIOD->BSRRL |= GPIO_BSRR_BS_15;
				break;
			default:
				return ERROR_INVALID_ARGUMENT;
		}
	}
	else if (set == LED_DISABLE)
	{
		switch (index)
		{
			case 0: //LD3 (PD13)
				GPIOD->BSRRH |= GPIO_BSRR_BS_13;
				break;
			case 1: //LD4 (PD12)
				GPIOD->BSRRH |= GPIO_BSRR_BS_12;
				break;
			case 2: //LD5 (PD14)
				GPIOD->BSRRH |= GPIO_BSRR_BS_14;
				break;
			case 3: //LD6 (PD15)
				GPIOD->BSRRH |= GPIO_BSRR_BS_15;
				break;
			default:
				return ERROR_INVALID_ARGUMENT;
		}
	}
	else if (set == LED_TOGGLE)
	{
		switch (index)
		{
			case 0: //LD3 (PD13)
				GPIOD->ODR ^= GPIO_ODR_ODR_13;
				break;
			case 1: //LD4 (PD12)
				GPIOD->ODR ^= GPIO_ODR_ODR_12;
				break;
			case 2: //LD5 (PD14)
				GPIOD->ODR ^= GPIO_ODR_ODR_14;
				break;
			case 3: //LD6 (PD15)
				GPIOD->ODR ^= GPIO_ODR_ODR_15;
				break;
			default:
				return ERROR_INVALID_ARGUMENT;
		}
	}
	else
	{
		return ERROR_INVALID_ARGUMENT;
	}

	return ERROR_NONE;
}
