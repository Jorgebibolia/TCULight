/**
  ******************************************************************************
  * @file           : sim7600.h
  * @brief          : Header for sim7600.c file.
  *                   This file contains the common defines of SIM7600 module
  ******************************************************************************
  * @developer      :	
	* @version        : version 0.0 
  *
  ******************************************************************************
  */
	
#ifndef _SIM7600_H_
#define _SIM7600_H_	

#include "stdint.h"

#define GSM_PW_EN_Pin GPIO_PIN_2
#define GSM_PW_EN_GPIO_Port GPIOC
#define GSM_POWERKEY_Pin GPIO_PIN_3
#define GSM_POWERKEY_GPIO_Port GPIOC
#define GSM_RI_Pin GPIO_PIN_2
#define GSM_RI_GPIO_Port GPIOA
#define GSM_DTR_Pin GPIO_PIN_3
#define GSM_DTR_GPIO_Port GPIOA

#define SIM7600_POW_ENABLE      HAL_GPIO_WritePin(GSM_PW_EN_GPIO_Port, GSM_PW_EN_Pin, GPIO_PIN_SET);
#define SIM7600_POW_DISABLE     HAL_GPIO_WritePin(GSM_PW_EN_GPIO_Port, GSM_PW_EN_Pin, GPIO_PIN_RESET);
#define SIM7600_POWKEY_ENABLE   HAL_GPIO_WritePin(GSM_POWERKEY_GPIO_Port, GSM_POWERKEY_Pin, GPIO_PIN_SET);
#define SIM7600_POWKEY_DISABLE  HAL_GPIO_WritePin(GSM_POWERKEY_GPIO_Port, GSM_POWERKEY_Pin, GPIO_PIN_RESET);


#define SIM7600_CHECK_COUNT   10

#define SIM7600_MODE          2 /*0:*allow the using of sms
                                  1:*allow the using of https                                  
																	2: allow the using of mqtt*/


void sim7600_initialize(void);
void send_at_command(void);
void sim7600_state_check(void);
void sim7600_set_sms_data(uint8_t *data, int data_len);
void sim7600_config_https(void);
void sim7600_set_https_data(uint8_t *data, int data_len);

void sim7600_conn_mqtt_server(void);
void sim7600_disconn_mqtt_server(void);
void sim7600_set_mqtt_data(uint8_t *data, int data_len);	
	
#endif //_SIM7600_H_
	
	
