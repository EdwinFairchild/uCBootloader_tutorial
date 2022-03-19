#ifndef _FRAMES_H_
#define _FRAMES_H_

/**
 * @brief Frame Format
 *          -------------------------------------------
 *          |-SOF-|-Frame_ID-|-Len-|-Data-|-CRC-|-EOF-|
 *  Bytes:  ---4-------4--------2-----n------4-----4--
 * 
 * STM32F4 n = 16 
 */

//delimeters
#define BL_START_OF_FRAME  0x45444459 //EDDY 
#define BL_END_OF_FRAME    0x46414952 //FAIR 

//frame IDS
#define BL_HEADER       0xFEEDEDDE //sends info about firmware: size , version 
#define BL_STATUS_CHECK 0x4b4b4b4b
#define BL_START_UPDATE 0xBA5EBA11
#define BL_PAYLOAD      0xDEADBEEF
#define BL_UPDATE_DONE  0xDEADDADE
#define BL_ACK_FRAME    0x45634AED
#define BL_NACK_FRAME   0x43636AEA
#define PAYLOAD_LEN 16

//frame formated struct
typedef struct __attribute__((packed))
{
    uint32_t start_of_frame;
    uint32_t frame_id;
    uint16_t payload_len;
    uint8_t payload[PAYLOAD_LEN];
    uint32_t crc32; 
    uint32_t  end_of_frame;

}frame_format_t;

//TODO: struct to configure bootloader


//states
typedef enum bootloader_state
{
    STATE_IDLE = 0,
    STATE_START_UPDATE,
    STATE_UPDATING,
}bootloader_state ; 

typedef enum
{
    BL_status_ok = 0,
    BL_status_err
}bl_status;

frame_format_t (*bootloader_state_functions[3])(void);


#endif