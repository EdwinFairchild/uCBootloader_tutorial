#include "bootloader.h"
#include "frames.h"
//location of reset handler for user_app
#define USER_APP_LOCATION (0x8020000 + 4)
uint32_t start_address = 0x8020000;


bool parse = false;
uint8_t bytes_buff[sizeof(frame_format_t)] = {0};
frame_format_t receivedFrame; 
frame_format_t ackFrame;
frame_format_t nackFrame;

bootloader_state bootloader_current_state = STATE_IDLE;
//-----------------------| prototypes |-----------------------
static void print(char *msg, ...);
static void jump_to_user_app(void);
static void uart_send_data(uint8_t *data, uint16_t len);
static bool parse_frame(void);
void bootloaderInit(void);
frame_format_t idle_state_func(void);
frame_format_t updating_state_func(void);
static void reset_recevied_frame(void);
static void set_bl_state(bootloader_state state);
static void sendFrame(frame_format_t *frame);
void erase_sector(void);
static void write_payload(void);
//-------------------------------------------------------------------
void bootloader_main(void)
{
	// TODO: fix :enable RX interrupt
	USART2->CR1 |= USART_CR1_RXNEIE;
	bootloaderInit();
	uint32_t timeNow = HAL_GetTick(); //current timestamp
	bootloader_state_functions[STATE_IDLE] = idle_state_func;
	//bootloader_state_functions[STATE_START_UPDATE] = start_update_state_func;
	bootloader_state_functions[STATE_UPDATING] = updating_state_func;

	while(1)
	{
		(*bootloader_state_functions[bootloader_current_state])();
        if(((HAL_GetTick() - timeNow) > 15000) && (bootloader_current_state == STATE_IDLE))
        {
        	jump_to_user_app();
        }

	}
}
//-------------------------------------------------------------------
void bootloaderInit(void)
{
	// create the ACK frame
	ackFrame.start_of_frame = BL_START_OF_FRAME;
	ackFrame.frame_id = BL_ACK_FRAME;
	ackFrame.payload_len = (uint16_t)65535;
	ackFrame.crc32 = 0xffffffff; // TODO:
	ackFrame.end_of_frame = BL_END_OF_FRAME;
	for (int i = 0; i < PAYLOAD_LEN; i++)
	{
		ackFrame.payload[i] = i;
	}

	// create the NACK frame
	nackFrame.start_of_frame = BL_START_OF_FRAME;
	nackFrame.frame_id = 0x45634AED;
	nackFrame.payload_len = (uint16_t)65535;
	nackFrame.crc32 = 0xffffffff; // TODO:
	nackFrame.end_of_frame = BL_END_OF_FRAME;
	for (int i = 0; i < PAYLOAD_LEN; i++)
	{
		nackFrame.payload[i] = i;
	}
}
//-------------------------------------------------------------------
frame_format_t idle_state_func(void)
{
	if (parse_frame())
	{

		switch (receivedFrame.frame_id)
		{
		case BL_START_UPDATE:
			set_bl_state(STATE_UPDATING);
			reset_recevied_frame();
			sendFrame(&ackFrame);
			break;

		// only states above are valid to switch out of idle state
		default:
			set_bl_state(STATE_IDLE);
			reset_recevied_frame();
		}
	}
	//return (frame_format_t)0;
	return ackFrame;
}
//-------------------------------------------------------------------
frame_format_t updating_state_func(void)
{
	// once we are updating for sure
	// we can go ahead and erase the required sectors only once
	static bool erased = false;
	if (parse_frame())
	{
		if (receivedFrame.frame_id == BL_PAYLOAD)
		{
			if (!erased) // only do this once
			{
				erase_sector();
				erased = true;
			}
			write_payload();
			// reset received frame
			//reset_recevied_frame();
			// send ack frame
			sendFrame(&ackFrame);
		}
		else if (receivedFrame.frame_id == BL_UPDATE_DONE)
		{
			jump_to_user_app();
		}
		// TODO: hanlde any unexpected frame
		// erase sector again
	}

	//return ackFrame;
}
//-------------------------------------------------------------------
static void write_payload(void)
{

	HAL_FLASH_Unlock();
	// TODO: add this to a config.h file
	for (int i = 0; i < 16; i += 4)
	{
		uint32_t *val = (uint32_t *)&receivedFrame.payload[i];
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start_address, *val);
		start_address += 4;
	}
	HAL_FLASH_Lock();
	// clear receivedFrame for next packet
	reset_recevied_frame();
	// TODO: read back the data and check crc

}
//-------------------------------------------------------------------
static void reset_recevied_frame(void)
{
	receivedFrame.start_of_frame = 0;
	receivedFrame.frame_id = 0;
	receivedFrame.payload_len = 0;
	receivedFrame.crc32 = 0;
	receivedFrame.end_of_frame = 0;
	for (int i = 0; i < 16; i++)
	{
		receivedFrame.payload[i] = 0;
	}
}
//-------------------------------------------------------------------
static void set_bl_state(bootloader_state state)
{
	bootloader_current_state = state;
}
//-------------------------------------------------------------------
static void jump_to_user_app(void)
{
	void (*user_app_reset_handler)(void) = (void *)(*((uint32_t *)(USER_APP_LOCATION)));
	user_app_reset_handler();
}
//-------------------------------------------------------------------
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
//-------------------------------------------------------------------
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
//-------------------------------------------------------------------
static void uart_send_data(uint8_t *data, uint16_t len)
{
	for (int i = 0; i < len; i++)
	{
		USART2->DR = data[i];
		while (!(USART2->SR & USART_SR_TXE))
			;
	}

	while (!(USART2->SR & USART_SR_TC))
		;
}
//-------------------------------------------------------------------
static void sendFrame(frame_format_t *frame)
{
	uint8_t *temp = NULL;
	temp = (uint8_t *)frame;
	uart_send_data(temp, sizeof(frame_format_t));
}
//-------------------------------------------------------------------
void bootloader_USART2_callback(uint8_t data)
{
	static uint8_t bytes_received_count = 0;
	// filll buffer until we have enough bytes to assemble a frame
	if (bytes_received_count <= sizeof(frame_format_t))
	{
		bytes_buff[bytes_received_count++] = data;
		if (bytes_received_count == sizeof(frame_format_t))
		{

			bytes_received_count = 0;
			parse = true;
		}
	}
}
//-------------------------------------------------------------------
static bool parse_frame(void)
{
	// checks if we have a frame to parse
	if (parse)
	{
		parse = false;
		// assemble a frame from bytes_buff
		memcpy(&receivedFrame, bytes_buff, sizeof(frame_format_t));
		// clear bytes buffer
		memset(bytes_buff, 0, sizeof(frame_format_t));
		// the type of frame we get will dictate what the next state should be
		if (receivedFrame.start_of_frame == BL_START_OF_FRAME && receivedFrame.end_of_frame == BL_END_OF_FRAME)
		{
			// TODO: check CRC
			// if frame is valid
			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------
