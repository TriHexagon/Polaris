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

#include <drivers/sd.h>

#define CMD_GO_IDLE_STATE 0
#define CMD_SEND_IF_COND 8
#define CMD_CMD_APP 55

static uint32_t response[4];

typedef enum RESPONSE_TYPE
{
	RESPONSE_NONE, RESPONSE_SHORT, RESPONSE_LONG
} RESPONSE_TYPE;

typedef enum SD_ERROR
{
	SD_ERROR_NONE, SD_ERROR_NO_RESPONSE, SD_ERROR_CRC_FAILED,
} SD_ERROR;

/********** device specific code begin **********/
#if DEVICE != stm32f4discovery
#error "Wrong device"
#endif

#define STM32F40_41xxx
#include <stm32f4xx.h>

static void initInterface(void)
{
	/********** initialize GPIO *********/
	//Ports in use: PORTC, PORTD
	//Pins in use: PD2 (CMD), PC12 (CK), PC8 (D0), PC11 (D3/Card Detect)
	//Alternate function mode for: PD2, PC12, PC8; Input mode for PC11
	//push-pull, high-speed, no pull-up/pull-down (extern pull-up)
	uint32_t tmp;

	//enable clock for ports
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN;

	//set pin mode (alternate function/input mode)
	tmp = GPIOC->MODER;
	tmp &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
	tmp |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_1;
	GPIOC->MODER = tmp;

	GPIOD->MODER &= ~GPIO_MODER_MODER2;

	//set output mode (push-pull)
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12);
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_2;

	//set speed mode (high speed)
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;

	//set no pull mode
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12);
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR2;

	//set alternate funtion selector (SDIO -> AF12)
	tmp = GPIOC->AFR[1];
	tmp &= ~( 0x0F /* PC8 */ | (0x0F<<16) /* PC12 */ );
	tmp |= 12 | (12<<16);
	GPIOC->AFR[1] = tmp;

	tmp = GPIOD->AFR[0];
	tmp &= ~(0x0F<<8); //PD2
	tmp |= 12<<8;
	GPIOD->AFR[0] = tmp;

	/*********** initialize SDIO **********/
	//enable clock for SDIO
	RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;

	//power off clock
	SDIO->POWER &= ~SDIO_POWER_PWRCTRL;

	//set clock settings (no HW flow, 1-bit bus mode, enable clock)
	//clock = 200kHz, CLKDIV = SDIOCLK/SDIOCK -2 = 48MHz/200kHz -2 =238
	tmp = SDIO->CLKCR;
	tmp &= ~(SDIO_CLKCR_HWFC_EN | SDIO_CLKCR_NEGEDGE | SDIO_CLKCR_WIDBUS | SDIO_CLKCR_BYPASS | SDIO_CLKCR_PWRSAV | SDIO_CLKCR_CLKDIV);
	tmp |= SDIO_CLKCR_CLKEN | 238;

	//disable SDIO interrupts
	SDIO->MASK = 0;

	//power-on clock for SDIO
	SDIO->POWER |= SDIO_POWER_PWRCTRL;
}

static SD_ERROR sendCmdNoData(uint8_t cmd, uint32_t argument, RESPONSE_TYPE type)
{
	//set argument
	SDIO->ARG = argument;

	if (type == RESPONSE_NONE)
	{
		//enable command path state machine, no response, set command index
		SDIO->CMD = SDIO_CMD_CPSMEN | cmd;

		//wait for command sent flag
		while ( !(SDIO->STA & SDIO_STA_CMDSENT) )
			;
	}
	else if (type == RESPONSE_SHORT)
	{
		//enable command path state machine, short response, set command index
		SDIO->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_WAITRESP_0 | cmd;

		//wait until command response is received (CRC check passed/failed) or is timeout
		while ( !(SDIO->STA & (SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL | SDIO_STA_CTIMEOUT)) )
			;

		//if timeout, return error
		if (SDIO->STA & SDIO_STA_CTIMEOUT)
			return SD_ERROR_NO_RESPONSE;

		//copy response
		response[0] = SDIO->RESP1;

		//if CCR check failed, return error
		if (SDIO->STA & SDIO_STA_CCRCFAIL)
			return SD_ERROR_CRC_FAILED;
	}
	else //type == RESPONE_LONG
	{
		//enable command path state machine, long response, set command index
		SDIO->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_WAITRESP | cmd;

		//wait until command response is received (CRC check passed/failed) or is timeout
		while ( !(SDIO->STA & (SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL | SDIO_STA_CTIMEOUT)) )
			;

		//if timeout, return error
		if (SDIO->STA & SDIO_STA_CTIMEOUT)
			return SD_ERROR_NO_RESPONSE;

		//copy response
		response[0] = SDIO->RESP4;
		response[1] = SDIO->RESP3;
		response[2] = SDIO->RESP2;
		response[3] = SDIO->RESP1;

		//if CCR check failed, return error
		if (SDIO->STA & SDIO_STA_CCRCFAIL)
			return SD_ERROR_CRC_FAILED;
	}

	return SD_ERROR_NONE;
}

/********** device specific code end **********/

error_t sd_init(void)
{
	initInterface();

	//CMD0 go idle state
	sendCmdNoData(CMD_GO_IDLE_STATE, 0, RESPONSE_NONE);

	//CMD8 (required for SDHC and SDXC cards) 2.7-3.6V voltage range, check pattern 0x11
	if ( sendCmdNoData(CMD_SEND_IF_COND, 0x111, RESPONSE_SHORT) == SD_ERROR_NO_RESPONSE )
	{
		//V2.00 or later SD card (voltage mismatch) or V1.x SD card or not SD card

		//CMD55 required for ACMD41, default RCA = 0x0000
		if ( sendCmdNoData(CMD_CMD_APP, 0, RESPONSE_SHORT) != SD_ERROR_NONE )
			return ERROR_NO_SDCARD; //no SD card


	}
}
