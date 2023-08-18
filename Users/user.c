/**
  ******************************************************************************
  * @file           : user.c
  * @brief          : source for user management.
	*                   This file contains the source code of user management.
	*
  ******************************************************************************
  * @developer      :	
	* @version        : version 0.0 
  *
  ******************************************************************************
  */
#include "user.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "cmsis_os.h"
#include "sim7600.h"

extern osSemaphoreId_t can_com_semaHandle;
extern osSemaphoreId_t sim7600_semaHandle;
extern CAN_HandleTypeDef hcan1;
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
Device_Status device_status;

uint8_t CAN_Tx_Data[8];
uint8_t CAN_Rx_Data[8];
uint32_t TxMailbox;
uint32_t tx_pending_cnt = 0;

Data_Buffer data_buffer;

void can_com_initialize(void)
{
	CAN_COM_ENABLE;
	
	HAL_CAN_Start(&hcan1);
	
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_RX_FIFO1_MSG_PENDING);
	
	device_status.can_com = CAN_OK; 
}
/**
  * @brief  user button function.
  * @param  None
  * @retval None 	
  */
void user_btn_event(void)
{
#if SIM7600_MODE == 0
	uint8_t can_send_data[8] = {0x01, 0x01, 0x03, 0x03, 0x03, 0x08, 0x09, 0x10};   /*to test*/
  can_com_send_data(can_send_data, CAN_DAT_MAX_LENGTH);
#elif SIM7600_MODE == 2	
	device_status.sim7600 = SIM7600_MQTT;   /*to test*/
  osSemaphoreRelease(sim7600_semaHandle);	/*to test*/
#endif
}
/**
  * @brief  send the data through CAN BUS.
  * @param  uint8_t *data - send data. 
  * @param  uint8_t dataLen- the length of send data.
  * @retval None 	
  */
void can_com_send_data(uint8_t *data, int dataLen)
{
	if(device_status.can_com == CAN_ERR||
		 device_status.can_com == CAN_TX_PENDING) return;
	
	TxHeader.DLC = dataLen>CAN_DAT_MAX_LENGTH?CAN_DAT_MAX_LENGTH:dataLen ;             
	TxHeader.IDE = CAN_ID_STD;           /*Standard Id*/
	TxHeader.RTR = CAN_RTR_DATA;         /*Data frame */
	TxHeader.StdId = CAN_PERIPH_STD_ID;
	
	memcpy(CAN_Tx_Data, data, dataLen>CAN_DAT_MAX_LENGTH? CAN_DAT_MAX_LENGTH:dataLen);
  device_status.can_com = CAN_TX_DATA;	
	osSemaphoreRelease(can_com_semaHandle);
}

/**
  * @brief  CAN communication task handle.
  * @param  None 
  * @retval None 	
  */
void can_com_task_handle(void)
{
	switch(device_status.can_com)
	{
		case CAN_OK:
			printf("the CAN communication was initialized.\n\r");
			break;
		case CAN_RX_DATA:
			if(data_buffer.temp_len + RxHeader.DLC <= SEND_BUFFER_SIZE)
			{
				memcpy(&data_buffer.temp_buffer[data_buffer.temp_len], CAN_Rx_Data, RxHeader.DLC);
		    data_buffer.temp_len += RxHeader.DLC;
				//printf("the data was loaded to tempbuffer as size %d.\n\r", data_buffer.temp_len);	
        if(data_buffer.temp_len == SEND_BUFFER_SIZE)
				{
					memcpy(data_buffer.send_buffer, data_buffer.temp_buffer, SEND_BUFFER_SIZE);
					data_buffer.send_len = SEND_BUFFER_SIZE;
					data_buffer.temp_len = 0;
					data_buffer.is_ready = true;
					
       #if SIM7600_MODE == 1
	        device_status.sim7600 = SIM7600_HTTPS;
       #elif SIM7600_MODE == 0 
          device_status.sim7600 = SIM7600_SMS;
       #else
          device_status.sim7600 = SIM7600_MQTT;					
       #endif				
          osSemaphoreRelease(sim7600_semaHandle);
          //printf("simtask was released.\n\r");					
				}				
			}
			else 
			{
				int temp_len = SEND_BUFFER_SIZE - data_buffer.temp_len;
				memcpy(&data_buffer.temp_buffer[data_buffer.temp_len], CAN_Rx_Data, temp_len);
				memcpy(data_buffer.send_buffer, data_buffer.temp_buffer, SEND_BUFFER_SIZE);
				data_buffer.send_len = SEND_BUFFER_SIZE;
				data_buffer.is_ready = true;
				
			 #if SIM7600_MODE == 1
	        device_status.sim7600 = SIM7600_HTTPS;
       #elif SIM7600_MODE == 0 
          device_status.sim7600 = SIM7600_SMS;
       #else	
          device_status.sim7600 = SIM7600_MQTT;				
       #endif	
				osSemaphoreRelease(sim7600_semaHandle);
				
				data_buffer.temp_len = 0;
				memcpy(&data_buffer.temp_buffer[data_buffer.temp_len], &CAN_Rx_Data[temp_len], RxHeader.DLC - temp_len);
        data_buffer.temp_len += RxHeader.DLC - temp_len;
        //printf("the data was loaded to buffer1 as size %d.\n\r", data_buffer.temp_len);					
			}			
			if(data_buffer.temp_len != 0)printf("the data was loaded to tempbuffer as size %d.\n\r", data_buffer.temp_len);	
			break;
		case CAN_RX_PASS:
			printf("the data was passed to other device\n\r");
			break;
		case CAN_TX_DATA:
			HAL_CAN_AddTxMessage(&hcan1, &TxHeader, CAN_Tx_Data, &TxMailbox);		
		  while(HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox))
			{
				tx_pending_cnt++;
				printf("the data is pending now.\n\r");
        device_status.can_com = CAN_TX_PENDING;				
				if(tx_pending_cnt > CAN_TX_PENDING_COUNT) {
					tx_pending_cnt = 0;
					device_status.can_com = CAN_ERR;
					/*have to insert the code to manage the error.*/
					break;
				}
			}
			if(device_status.can_com != CAN_ERR)
			{
				device_status.can_com = CAN_OK;
				printf("the data was sent to other device.\n\r");
			}
			break;
		default:
			break;
	}	
}
/**
  * @brief  implementint of Rx FIFO 0 message pending callback
  * @param  CAN_HandleTypeDef *hcan - hcan pointer to a CAN_HandleTypeDef structure.  
  * @retval None 	
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, CAN_Rx_Data) != HAL_OK)
	{
    device_status.can_com = CAN_ERR;
    /*have to insert the code to manage the error.*/
	}
	else
	{
		if(RxHeader.StdId == CAN_DEVICE_STD_ID)
		{
			device_status.can_com = CAN_RX_DATA;			
		}
		else
		{
			device_status.can_com = CAN_RX_PASS;
		}
    osSemaphoreRelease(can_com_semaHandle);		
	}	
	printf("the data was received from CAN, data length is %d\n\r", RxHeader.DLC);	
}
uint8_t temp_data[5] = {0x30,};
/**
  * @brief  sim7600 task handle.
  * @param  None 
  * @retval None 	
  */
void sim7600_task_handle(void)
{
	switch(device_status.sim7600)
	{
		case SIM7600_CHECK:
			sim7600_state_check();
			break;
		case SIM7600_HTTPS:
			sim7600_set_https_data(data_buffer.send_buffer, data_buffer.send_len);
			break;
		case SIM7600_SMS:
			sim7600_set_sms_data(data_buffer.send_buffer, data_buffer.send_len);
			break;
		case SIM7600_MQTT:
			sim7600_set_mqtt_data(temp_data, 0);
			break;
		default:
			break;
	}
}

