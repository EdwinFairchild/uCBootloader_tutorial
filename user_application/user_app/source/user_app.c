#include "user_app.h"
#include "main.h"
#include "gpio.h"
//prototype
static void print(char *msg, ...);
void user_app(void)
{
	while(1)
	{
		print("Youtube Tutorial:  %d\r\n", HAL_GetTick());
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		HAL_Delay(75);


	}
}

static void print(char *msg, ...)
{
	char buff[250];
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);

	for (int i = 0; i < strlen(buff); i++)
	{
		USART2->DR = buff[i];
		while (!(USART2->SR & USART_SR_TXE))
			;
	}

	while (!(USART2->SR & USART_SR_TC))
		;
}
