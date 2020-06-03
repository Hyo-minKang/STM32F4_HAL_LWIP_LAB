/*
 * wizInterface.h
 *
 *  Created on: 2020. 5. 31.
 *      Author: eziya76@gmail.com
 */

#ifndef INC_WIZINTERFACE_H_
#define INC_WIZINTERFACE_H_

#include "main.h"

void WIZ_SPI_Select(void);
void WIZ_SPI_Deselect(void);
void WIZ_SPI_TxByte(uint8_t byte);
uint8_t WIZ_SPI_RxByte(void);

#endif /* INC_WIZINTERFACE_H_ */