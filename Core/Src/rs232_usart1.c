/*
 * rs232_usart2.c
 *
 *  Created on: 2023年4月10日
 *      Author: 陈骏骏
 */

#include "rs232_usart1.h"

//RS232接收数据缓存
uint8_t pRS232RxBuff[RS232_MAXSIZE];
