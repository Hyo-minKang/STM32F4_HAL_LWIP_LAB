/*
 * tcpclient.c
 *
 *  Created on: 2020. 5. 31.
 *      Author: eziya76@gmail.com
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "tcpclient.h"
#include "wizInterface.h"

#include <stdio.h>

#define SERVER_IP1	192	//server ip
#define SERVER_IP2	168
#define SERVER_IP3	1
#define SERVER_IP4	227
#define SERVER_PORT	1234 //server port

static uint8_t serverIP[] = { SERVER_IP1, SERVER_IP2, SERVER_IP3, SERVER_IP4 };
static uint16_t localPort = 50000;

//tcp client task
void StartWizTcpClientTask(void const *argument) {

	if(!WIZ_ChipInit())
	{
		printf("WIZ_ChipInit failed.\n");
		vTaskDelete(NULL);	//clear task
	}

	if(!WIZ_NetworkInit())
	{
		printf("WIZ_NetworkInit failed.\n");
		vTaskDelete(NULL);	//clear task
	}

	while (1) {
		int32_t ret;

		//create socket
		ret = socket(CLIENT_SOCKET, Sn_MR_TCP, localPort++, SF_TCP_NODELAY);
		if(localPort == 0xFFFF) {
			localPort = 50000;
		}

		if (ret < 0) {
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
			printf("socket failed{%ld}.\n", ret);
			close(CLIENT_SOCKET);
			osDelay(500);
			continue;
		}

		while(getSn_SR(CLIENT_SOCKET) != SOCK_INIT);

		//connect to the server
		//modified connect api to make a detour invalid error returns(SOCKERR_SOCKCLOSED)
		ret = connect(CLIENT_SOCKET, serverIP, SERVER_PORT);
		if (ret < 0) {
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
			printf("connect failed{%ld}.Sn_SR()={%d}\n", ret, getSn_SR(CLIENT_SOCKET));
			//modified connect api to make a detour infinite loop when Sn_SR returns 0x10
			close(CLIENT_SOCKET);
			osDelay(500);
			continue;
		}

		//prepare data to send & receive
		struct time_packet packet;
		memset(&packet, 0, sizeof(struct time_packet));
		packet.head = 0xAE;	//head
		packet.type = REQ; 	//request type
		packet.tail = 0xEA; //tail

		uint8_t failed = 0;
		int16_t written = 0;
		int16_t read = 0;

		//send request
		ret = send(CLIENT_SOCKET, (uint8_t*) (&packet + written), sizeof(struct time_packet) - written);
		if (ret < 0) {
			HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
			printf("send failed{%ld}.\n", ret);
			close(CLIENT_SOCKET); //unexpected close
			continue;
		}

		//receive response
		while (1) {
			ret = recv(CLIENT_SOCKET, (uint8_t*) (&packet + read), sizeof(struct time_packet) - read);
			if (ret < 0) {
				HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
				printf("recv failed.{%ld}\n", ret);
				close(CLIENT_SOCKET); //unexpected close
				failed = 1;
				break;
			}

			read += ret;
			if (read >= sizeof(struct time_packet))	//overflow
				break;
		}

		if (failed)
			continue; //start again

		 //if received length is valid, print time information & toggle led
		if (read == sizeof(struct time_packet) && packet.type == RESP)
				{
			printf("%04d-%02d-%02d %02d:%02d:%02d\n",
					packet.year + 2000, packet.month, packet.day,
					packet.hour, packet.minute, packet.second);
			HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
		}
		close(CLIENT_SOCKET);		//close socket
		osDelay(10);
	}

	vTaskDelete(NULL);	//clear task
}
