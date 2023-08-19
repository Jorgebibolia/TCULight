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
#include "cmsis_os.h"

extern UART_HandleTypeDef huart4;
extern Device_Status device_status;
uint32_t sim7600_check_cnt = 0;
char ATcommand[256];
char mobileNumber[] = "+8613650003455"; // exchange with your phone number.

char http_str1[] = "AT+HTTPPARA=\"URL\",";
char http_str2[] = "\"https://api.xxx.com/update?"; //exchange with your http information.
char http_str3[] = "api_key="; //add the key.
char http_str4[] = "&field="; 
char http_str5[] = "\"\r\n";

const char apn[]  = "\"IP\",\"CMNET\"";  //change this according to your sim card details
const char host[] = "tcp://me.paham.ai"; 
const int  port = 1883;
const char username[] = "\"SaeLayeXU43KlasNp0dN\"";
const char password[] = "\" \"";
const char topic[] = "v1/devices/me/telemetry";
char payload[] = "{\"temperature\":43}";

uint8_t sim_uart_rx[100];
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
	while(device_status.sim7600 != SIM7600_OK)
	{
		sim7600_check_cnt++;
    if(sim7600_check_cnt > SIM7600_CHECK_COUNT)
		{
			device_status.sim7600 = SIM7600_ERR;
			/*have to insert the code to manage the error.*/
      break;			
		}    		
		HAL_UART_Transmit(&huart4,(uint8_t *)ATcommand, strlen(ATcommand),1000);
    HAL_UART_Receive (&huart4, sim_uart_rx, 100, 1000);
    if(strstr((char *)sim_uart_rx, "OK"))
	  {
			HAL_GPIO_WritePin(USER_LED2_GPIO_Port, USER_LED2_Pin, GPIO_PIN_RESET);    	
      device_status.sim7600 = SIM7600_OK;
      printf("%.100s\n\r", sim_uart_rx);			
	  }
    memset(sim_uart_rx, 0, sizeof(sim_uart_rx));		
    HAL_Delay(1000);	
	}
 #if SIM7600_MODE == 1
	if(device_status.sim7600 == SIM7600_OK) sim7600_config_https();
	
 #elif SIM7600_MODE == 2
	sprintf(ATcommand, "AT+CGREG?\r\n");	
	device_status.sim7600 = SIM7600_MCHECK;
	sim7600_check_cnt = 0;
	while(device_status.sim7600 != SIM7600_OK)
	{
    if(sim7600_check_cnt > SIM7600_CHECK_COUNT)
		{
			device_status.sim7600 = SIM7600_ERR;
			/*have to insert the code to manage the error.*/
      break;			
		}
    sim7600_check_cnt++;		
		HAL_UART_Transmit(&huart4,(uint8_t *)ATcommand, strlen(ATcommand),1000);
    HAL_UART_Receive (&huart4, sim_uart_rx, 100, 1000);
    if(strstr((char *)sim_uart_rx,"+CGREG: 0,1")) 
    {
			HAL_GPIO_TogglePin(USER_LED2_GPIO_Port, USER_LED2_Pin);
			device_status.sim7600 = SIM7600_OK;
			printf("%.100s\n\r", sim_uart_rx);
    }	
    memset(sim_uart_rx, 0, sizeof(sim_uart_rx));		
    HAL_Delay(500);	
	}
  if(device_status.sim7600 == SIM7600_OK)	
	{
		sim7600_conn_mqtt_server();	
	}
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
	send_at_command();		
	HAL_Delay(1000);
	
	sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
	send_at_command();	
	HAL_Delay(100);	
		
	memcpy(ATcommand, (char*)data, data_len);
	sprintf(&ATcommand[data_len], "%c", 0x1A);
	send_at_command();		
	printf("the data was sent to phone through SIM7600.\n\r");
}
/**
  * @brief  connect to mqtt server. 
  * @param  None
  * @retval None 	
  */
void sim7600_conn_mqtt_server(void)
{	
	sprintf(ATcommand,"AT+CGDCONT=1,%s\r\n",apn);	//set the apn.
  send_at_command();
	HAL_Delay(2000);
	
	sprintf(ATcommand, "AT+CMQTTSTART\r\n");  //the start of mqtt service.
  send_at_command();
	HAL_Delay(2000);
	
	sprintf(ATcommand, "AT+CMQTTACCQ=0,\"element123\",0,4\r\n"); //acquire a client	
  send_at_command();
	HAL_Delay(2000);
	
  //connect to custom mqtt server with user, password.
	sprintf(ATcommand,"AT+CMQTTCONNECT=0,\"%s:%d\",90,1,%s,%s\r\n",host, port, username, password);  
  send_at_command();	
}
/**
  * @brief  disconnect from mqtt server. 
  * @param  None
  * @retval None 	
  */
void sim7600_disconn_mqtt_server(void)
{
	sprintf(ATcommand,"AT+CMQTTDISC=0,120\r\n"); //disconnect from mqtt server
	send_at_command();
	
	sprintf(ATcommand,"AT+CMQTTREL=0\r\n");      //release the client.
	send_at_command();
	
	sprintf(ATcommand,"AT+CMQTTSTOP\r\n");       //stop mqtt service.
  send_at_command();
}
/**
  * @brief  send the data thr mqtt. 
  * @param  None
  * @retval None 	
  */
void sim7600_set_mqtt_data(uint8_t *data, int data_len)
{ 
	sprintf(ATcommand,"AT+CMQTTTOPIC=0,%d\r\n",strlen(topic)); // set the topic.
  send_at_command();	

  sprintf(ATcommand,"%s\r\n",topic); //send the content of topic.
  send_at_command();

	sprintf(ATcommand,"AT+CMQTTPAYLOAD=0,%d\r\n",strlen(payload)); //set the payload(to testing).
	//sprintf(ATcommand,"AT+CMQTTPAYLOAD=0,%d\r\n",data_len); //have to use this code with CAN communication.
  send_at_command();
	
	memset(sim_uart_rx,0,sizeof(sim_uart_rx)); //send the contents of payload.
	HAL_UART_Transmit(&huart4,(uint8_t *)payload, strlen(payload),1000);  // send the contents of payload(to testing).
	//HAL_UART_Transmit(&huart4,(uint8_t *)data, data_len,1000);   //have to use this code with CAN communication	
	HAL_UART_Receive (&huart4, sim_uart_rx, 100, 1000);
	printf("%.100s\n\r", sim_uart_rx);

	sprintf(ATcommand,"AT+CMQTTPUB=0,1,60\r\n"); //Publish
  send_at_command();	
}
/**
  * @brief  configuration of https. 
  * @param  None
  * @retval None 	
  */
void sim7600_config_https(void)
{	
	sprintf(ATcommand, "AT+CCID");
	send_at_command();
	
	sprintf(ATcommand, "AT+CREG?");
	send_at_command();
	
	sprintf(ATcommand, "AT+CGATT=1");
	send_at_command();
	
	sprintf(ATcommand, "AT+CGACT=1,1");
	send_at_command();
	
	sprintf(ATcommand, "AT+CGDCONT=1,\"IP\",\"apn\"");
	send_at_command();
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
	send_at_command();	
	
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
		
	send_at_command();	
		
	sprintf(ATcommand, "AT+HTTPACTION=0\r\n");
	send_at_command();	
	sprintf(ATcommand, "AT+HTTPTERM\r\n");
	send_at_command();	
}
/**
  * @brief  send AT Command to SIM7600 module. 
  * @param  None
  * @retval None 	
  */
void send_at_command(void)
{
  memset(sim_uart_rx,0,sizeof(sim_uart_rx));
  HAL_UART_Transmit(&huart4,(uint8_t *)ATcommand, strlen(ATcommand),1000);
  HAL_UART_Receive (&huart4, sim_uart_rx, 100, 100);
	printf("%.100s\n\r", sim_uart_rx);
} 

