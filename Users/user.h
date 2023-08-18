/**
  ******************************************************************************
  * @file           : user.h
  * @brief          : Header for user.c file.
  *                   This file contains the common defines of user management
  ******************************************************************************
  * @developer      :	
	* @version        : version 0.0 
  *
  ***
	*/
	
#ifndef _USER_H_
#define _USER_H_

#include "stdint.h"
#include "stdbool.h"

#define CAN_STBY_Pin GPIO_PIN_10
#define CAN_STBY_GPIO_Port GPIOA

#define CAN_COM_ENABLE        HAL_GPIO_WritePin(CAN_STBY_GPIO_Port, CAN_STBY_Pin, GPIO_PIN_RESET);
#define CAN_COM_DISABLE       HAL_GPIO_WritePin(CAN_STBY_GPIO_Port, CAN_STBY_Pin, GPIO_PIN_SET);

#define DATA_BUFFER_SIZE             4096
#define SEND_BUFFER_SIZE             128
typedef struct _Device_Status
{
	uint8_t  sim7600;    /*!< Specifies the sim7600 status.                 0x00 ~ 0x7F*/                         
  uint8_t  can_com;    /*!< Specifies the CAN communication status.       0x00 ~ 0x7F*/
	
}Device_Status;

typedef struct _DATA_BUFFER
{
	bool      is_ready;
	uint16_t  temp_len;                      /*!< Specifies the length of temp buffer.       0x00 ~ 0x80*/
	uint16_t  send_len;                      /*!< Specifies the length of send buffer.       0x00 ~ 0x80*/
	uint8_t   temp_buffer[DATA_BUFFER_SIZE];
	uint8_t   send_buffer[SEND_BUFFER_SIZE];	
}Data_Buffer;

typedef enum {
	SIM7600_OK      = 0, 
  SIM7600_ERR     = 1,	
	SIM7600_CHECK   = 2,
	SIM7600_MCHECK  = 3,
	SIM7600_HTTPS 	= 4,
	SIM7600_MQTT    = 5,
	SIM7600_SMS  	  = 6,  
} SIM7600_SATUS;

typedef enum {
	CAN_OK          = 0,
  CAN_ERR         = 1,	
	CAN_RX_DATA 		= 2,
	CAN_RX_PASS     = 3,
	CAN_TX_DATA  	  = 4, 
	CAN_TX_PENDING  = 5,  	
} CAN_STATUS;


#define CAN_DEVICE_STD_ID            0x000  
#define CAN_PERIPH_STD_ID            0x005  //This value must be determine by other CAN device.
#define CAN_DAT_MAX_LENGTH           8
#define CAN_TX_PENDING_COUNT         0xFFF 


void can_com_initialize(void);
void can_com_send_data(uint8_t *data, int dataLen);
void can_com_task_handle(void);

void sim7600_task_handle(void);

void user_btn_event(void);

#endif //_USER_H_


