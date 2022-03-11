#include "bootloader.h"

//location of reset handler for user_app
#define USER_APP_LOCATION (0x8020000 + 4)
#define SECTOR_6 (0x08040000)
volatile uint32_t *myHexWord = (volatile uint32_t *)SECTOR_6 ;
//prototype
static void print(char *msg, ...);
static void jump_to_user_app(void);

void bootloader_main(void)
{
	uint32_t timeNow = HAL_GetTick(); //current timestamp

	while(1)
	{
		print("tick: %d\r\n", HAL_GetTick());
		if((HAL_GetTick() - timeNow) >= 5000) //5s
		{
			jump_to_user_app();
		}

	}
}


static void jump_to_user_app(void)
{
	void (*user_app_reset_handler)(void) = (void *)(*((uint32_t *)(USER_APP_LOCATION)));
	user_app_reset_handler();
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

// TODO:  abstract sector erasing based user app memory location and size
void erase_sector(void)
{
	FLASH_EraseInitTypeDef erase;
	erase.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase.NbSectors = 1;
	erase.Sector = FLASH_SECTOR_5;
	erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	uint32_t err = 0;

	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase, &err);
	HAL_FLASH_Lock();
	// TODO: check return of FLASH ERASE and handle it.
}
