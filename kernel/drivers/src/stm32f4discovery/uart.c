#include <drivers/uart.h>

#define STM32F40_41xxx
#include <stm32f4xx.h>

static bool isInit = false;

bool uart_isInit(void)
{
	return isInit;
}

error_t uart_init(void)
{
	//setup GPIO USART1_TX (PA9 or PB6), USART1_RX (PA10 or PB7) PB
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
	SET_BIT(GPIOB->MODER, GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
	SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1);
	SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR6_1);
	SET_BIT(GPIOB->AFR[0], (7<<(4*6)) | (7<<(4*7)));

	//setup USART1 (APB2 84MHz)
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
	SET_BIT(USART1->CR1, USART_CR1_UE); //enable usart
	//program M bit in CR1
	//stop bits CR2
	//baud rate BRR
	USART1->BRR = 0x222E;
	//set TE in CR1 (idel frame as first transmition)
	SET_BIT(USART1->CR1, USART_CR1_TE);

	isInit = true;

	return ERROR_NONE;
}

void uart_send(const void* data, size_t size)
{
	const uint8_t* ptr = data;
	for (size_t i = 0; i < size; i++)
	{
		USART1->DR = ptr[i];
		while ( !(USART1->SR & USART_SR_TXE) )
			;
	}

	while ( !(USART1->SR & USART_SR_TC) )
		;
}
