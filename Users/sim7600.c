/**
  ******************************************************************************
  * @file           : sim7600.c
  * @brief          : source for sim7600 module.
	*                   This file contains the source code of sim7600 module management.
	*
  ******************************************************************************
  * @developer      :	
	* @version        : version 0.0 
  *
  ******************************************************************************
  */
#include "sim7600.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stm32f4xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef huart4;
extern Device_Status device_status;
uint32_t sim7600_check_cnt = 0;
char ATcommand[256];
char mobileNumber[] = "+8613655520565"; // have to exchange.

char http_str1[] = "AT+HTTPPARA=\"URL\",";
char http_str2[] = "\"https://api.xxx.com/update?"; //have to exchange.
char http_str3[] = "api_key="; //have to add the key
char http_str4[] = "&field="; 
char http_str5[] = "\"\r\n";

uint8_t sim_uart_rx[30] = {30};
/**
  * @brief  initialize of sim7600 module.
  * @param  None
  * @retval None 	
  */
void sim7600_initialize(void)
{
	SIM7600_POW_ENABLE;
	SIM7600_POWKEY_ENABLE;
	HAL_Delay(15000);
  device_status.sim7600 = SIM7600_CHECK;	
}

/**
  * @brief  check the status of sim7600 module.
  * @param  None
  * @retval None 	
  */
void sim7600_state_check(void)
{
	sprintf(ATcommand, "AT\r\n");
	HAL_UART_Receive_IT(&huart4, sim_uart_rx, 30);
	while(device_status.sim7600 != SIM7600_OK)
	{
    if(sim7600_check_cnt > SIM7600_CHECK_COUNT)
		{
			device_status.sim7600 = SIM7600_ERR;
			/*have to insert the code to manage the error.*/
      break;			
		}			
		HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));		
    HAL_Delay(1000);	
	}
 #if SIM7600_MODE == 1
	if(device_status.sim7600 == SIM7600_OK) sim7600_config_https();
 #endif	
}
/**
  * @brief  send sms message to desired phone number. 
  * @param  None
  * @retval None 	
  */
void sim7600_set_sms_data(uint8_t *data, int data_len)
{
	for(int i = 0; i < data_len; i++) data[i]+=0x30;	
	
  sprintf(ATcommand, "AT+CMGF=1\r\n");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand,strlen(ATcommand));	
	HAL_Delay(1000);
	
	sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	HAL_Delay(100);	
		
	memcpy(ATcommand, (char*)data, data_len);
	sprintf(&ATcommand[data_len], "%c", 0x1A);
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));	
	printf("the data was sent to phone through SIM7600.\n\r");
}
/**
  * @brief  configuration of https. 
  * @param  None
  * @retval None 	
  */
void sim7600_config_https(void)
{	
	sprintf(ATcommand, "AT+CCID");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	sprintf(ATcommand, "AT+CREG?");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	sprintf(ATcommand, "AT+CGATT=1");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	sprintf(ATcommand, "AT+CGACT=1,1");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	sprintf(ATcommand, "AT+CGDCONT=1,\"IP\",\"apn\"");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
}
/**
  * @brief  send data to desired webserver. 
  * @param  None
  * @retval None 	
  */
void sim7600_set_https_data(uint8_t *data, int data_len)
{
  int https_str_len = 0;	
	for(int i = 0; i < data_len; i++) data[i]+=0x30;
	
	sprintf(ATcommand, "AT+HTTPINIT\r\n");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));	
	
	sprintf(ATcommand, http_str1, strlen(http_str1));
	https_str_len = strlen(http_str1);
	sprintf(&ATcommand[https_str_len], http_str2, strlen(http_str2));
	https_str_len+= strlen(http_str2);
	sprintf(&ATcommand[https_str_len], http_str3, strlen(http_str3));
	https_str_len+= strlen(http_str3);
	sprintf(&ATcommand[https_str_len], http_str4, strlen(http_str4));
	https_str_len+= strlen(http_str4);
	memcpy(&ATcommand[https_str_len], (char*)data, data_len);
	https_str_len+=data_len;
	sprintf(&ATcommand[https_str_len], http_str5, strlen(http_str5));
	https_str_len+= strlen(http_str5);
		
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, https_str_len);
		
	sprintf(ATcommand, "AT+HTTPACTION=0\r\n");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
	sprintf(ATcommand, "AT+HTTPTERM\r\n");
	HAL_UART_Transmit_IT(&huart4, (uint8_t *)ATcommand, strlen(ATcommand));
}

/**
  * @brief  implementint of uart interrupt callbacks
  * @param  UART_HandleTypeDef *huart
  * @retval None 	
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_Receive_IT(&huart4, sim_uart_rx, 30);
  if(strstr((char *)sim_uart_rx, "OK") && device_status.sim7600 == SIM7600_CHECK)
	{
		HAL_GPIO_WritePin(USER_LED2_GPIO_Port, USER_LED2_Pin, GPIO_PIN_RESET);    	
    device_status.sim7600 = SIM7600_OK;		
	}  
  memset(sim_uart_rx, 0, sizeof(sim_uart_rx));	
}








