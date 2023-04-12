/*
 * lin_usart1.c
 *
 *  Created on: Apr 8, 2023
 *      Author: 陈骏骏
 */

#include "lin_usart1.h"
#include "rs232_usart2.h"
#include "usart.h"
#include <string.h>

//LIN同步帧字节
uint8_t  SYNC_Frame = 0x55;

//LIN接收数据缓存
uint8_t pLINRxBuff[LIN_RX_MAXSIZE];
//LIN发送数据缓存
uint8_t pLINTxBuff[LIN_TX_MAXSIZE];
//当前测试的电机步长
uint16_t EXV_Test_Step;
//发送读取帧的标志位
uint8_t LIN_Read_Flag;

/****************************************************************************************
** 函数名称: LINCheckSum----标准校验
** 功能描述: 计算并返回LIN校验值
** 参    数:  uint8_t *buf：需要计算的数组
			        uint8_t lens：数组长度
** 返 回 值:   uint8_t ckm: 计算结果
****************************************************************************************/
uint8_t LIN_Check_Sum(uint8_t *buf, uint8_t lens)
{
    uint8_t i, ckm = 0;
    uint16_t chm1 = 0;
    for(i = 1; i < lens; i++)
    {
        chm1 += *(buf+i);
    }
    ckm = chm1 / 256;
    ckm = ckm + chm1 % 256;
    ckm = 0xFF - ckm;
    return ckm;
}
/****************************************************************************************
** 函数名称: LINCheckSumEn----增强校验
** 功能描述: 计算并返回LIN校验值
** 参    数:  uint8_t *buf：需要计算的数组
			        uint8_t lens：数组长度
** 返 回 值:   uint8_t ckm: 计算结果
****************************************************************************************/
uint8_t LIN_Check_Sum_En(uint8_t *buf, uint8_t lens)
{
    uint8_t i, ckm = 0;
    uint16_t chm1 = 0;
    for(i = 0; i < lens; i++)
    {
        chm1 += *(buf+i);
    }
    ckm = chm1 / 256;
    ckm = ckm + chm1 % 256;
    ckm = 0xFF - ckm;
    return ckm;
}
/****************************************************************************************
** 函数名称: Lin_Tx_PID_Data
** 功能描述: LIN发送数据帧
** 参    数: *buf:数组地址；buf[0]=PID
			       lens:数据长度,不含校验字节
			       CK_Mode: 校验类型增强型LIN_CK_ENHANCED=1：基本LIN_CK_STANDARD=0
             Timeout (0xffff)不做时间限制
** 返 回 值: 无
****************************************************************************************/
void LIN_Tx_PID_Data(UART_HandleTypeDef *huart, uint8_t *buf, uint8_t lens, LIN_CK_Mode CK_Mode)
{
    if(CK_Mode == LIN_CK_STANDARD)
    {
    	//arr[i] = *(arr + i)
		//计算标准型校验码，不计算PID
		*(buf + lens) = LIN_Check_Sum(buf, lens);
    }
    else
    {
    	//计算增强型校验码,连PID一起校验
		*(buf + lens) = LIN_Check_Sum_En(buf, lens);
    }

    //发送同步间隔段
	HAL_LIN_SendBreak(huart);
	//发送同步段
	HAL_UART_Transmit(huart,&SYNC_Frame,1,HAL_MAX_DELAY);
	//发送PID,数据内容和校验
	HAL_UART_Transmit(huart,buf,LIN_TX_MAXSIZE,HAL_MAX_DELAY);
}
/****************************************************************************************
** 函数名称: Lin_Tx_PID
** 功能描述: LIN发送报文头，PID，读取从机状态信息
** 参    数: PID, Timeout (0xffff)不做时间限制
** 返 回 值: 无
****************************************************************************************/
void LIN_Tx_PID(UART_HandleTypeDef *huart, uint8_t PID)
{
	//发送间隔帧
	HAL_LIN_SendBreak(huart);
	//发送同步帧
	HAL_UART_Transmit(huart,&SYNC_Frame,1,HAL_MAX_DELAY);
	HAL_UART_Transmit(huart,&PID,1,HAL_MAX_DELAY);
}

/**
 * 设置返回给RS232上位机的数据
 */
void Send_Resp_Data(uint8_t* pBuff,uint16_t data)
{
	*pBuff = data >> 8;
	*(pBuff + 1) = data;
	HAL_UART_Transmit(&huart2,pBuff,sizeof(data),HAL_MAX_DELAY);
	LIN_Read_Flag = DISABLE;
}

/**
 * 数据处理函数
 */
void LIN_Data_Process()
{
	LIN_Read_Flag = ENABLE;
	uint8_t RS232_Resp_Result[2] = {0};
	uint16_t EXV_Run_Step = 0;
	if(pLINRxBuff[2] == EXV_F_RESP_ERROR)
	{
		Send_Resp_Data(RS232_Resp_Result,RS232_RESP_LIN_COMM_ERROR);
	}
	else if(pLINRxBuff[5] > 0)
	{
		switch(pLINRxBuff[5])
		{
		case EXV_ST_FAULT_SHORTED:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_MC_SHORT);
			break;
		case EXV_ST_FAULT_OPENLOAD:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_MC_LOADOPEN);
			break;
		case EXV_ST_FAULT_OVERTEMP:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_SHUTDOWN);
			break;
		case EXV_ST_FAULT_ACTUATORFAULT:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_ACTUATOR_FAULT);
			break;
		}
	}
	else if(pLINRxBuff[6] > 0)
	{
		switch(pLINRxBuff[6])
		{
		case EXV_ST_VOLTAGE_OVER:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OVER_VOLTAGE);
			break;
		case EXV_ST_VOLTAGE_UNDER:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_UNDER_VOLTAGE);
			break;
		}
	}
	else if(pLINRxBuff[7] == EXV_OVERTEMP_OVER)
	{
		Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OVERTEMP);
	}
	else if(pLINRxBuff[3] == EXV_ST_RUN_NOT_MOVE)
	{
		EXV_Run_Step = (pLINRxBuff[9] << 8) | pLINRxBuff[8];
		if(EXV_Run_Step == EXV_Test_Step)
		{
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OK);
		}
		else
		{
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_ERROR);
		}
	}
	if(LIN_Read_Flag)
	{
		LIN_Tx_PID(&huart1, LIN_PID_35_0x23);
	}
}
