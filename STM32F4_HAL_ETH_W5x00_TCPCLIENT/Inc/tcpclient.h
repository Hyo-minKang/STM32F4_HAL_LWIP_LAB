/*
 * tcpclient.h
 *
 *  Created on: 2020. 5. 31.
 *      Author: eziya76@gmail.com
 */

#ifndef INC_TCPCLIENT_H_
#define INC_TCPCLIENT_H_

typedef enum
{
  REQ = 0,
  RESP = 1
} packet_type;

struct time_packet
{
  uint8_t head; //0xAE
  uint8_t type; //0:REQ, 1:RESP
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t dummy[247]; //you may add more information
  uint8_t tail; //0xEA
};

void StartWizTcpClientTask(void const *argument);

#endif /* INC_TCPCLIENT_H_ */
