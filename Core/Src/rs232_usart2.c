/*
 * rs232_usart2.c
 *
 *  Created on: 2023年4月10日
 *      Author: 陈骏骏
 */

#include "rs232_usart2.h"
#include "lin_usart1.h"
#include "usart.h"
#include <string.h>

//RS232接收数据缓存
uint8_t pRS232RxBuff[RS232_MAXSIZE];

/**
 * 数据处理函数
 */
void RS232_Data_Process()
{
	uint8_t index = 0;
	EXV_Test_Step = (pRS232RxBuff[0] << 8) | pRS232RxBuff[1];
	pLINTxBuff[index++] = LIN_PID_52_0x34;
	pLINTxBuff[index++] = pRS232RxBuff[1];
	pLINTxBuff[index++] = pRS232RxBuff[0];
	pLINTxBuff[index++] = EXV_MOVE_CMD;
	pLINTxBuff[index++] = EXV_INIT_NO_REQ;
	while(index < LIN_TX_MAXSIZE - 1)
	{
		pLINTxBuff[index++] = 0xFF;
	}
	LIN_Tx_PID_Data(&huart1,pLINTxBuff,index,LIN_CK_ENHANCED);
	LIN_Tx_PID(&huart1, LIN_PID_35_0x23);
	memset(pLINTxBuff,0,LIN_TX_MAXSIZE);
	memset(pRS232RxBuff,0,RS232_MAXSIZE);
}
