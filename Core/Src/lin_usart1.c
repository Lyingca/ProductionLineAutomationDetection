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
uint8_t LIN_Read_Flag = DISABLE;
//发送写帧的标志位
uint8_t LIN_Send_Flag = DISABLE;
//指令重复发送次数
uint8_t retries = 5;
//初始化LIN芯片信息
struct LIN_Chip_Msg chip[2] = {{0xF5,0xB4,0xFF,0xFC},{0x37,0x76,0xFF,0xFC}};
//芯片编号
uint8_t chip_Num;

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
		*(buf + lens) = LIN_Check_Sum(buf, LIN_CHECK_STD_NUM);
    }
    else
    {
    	//计算增强型校验码,连PID一起校验
		*(buf + lens) = LIN_Check_Sum_En(buf, LIN_CHECK_EN_NUM);
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
 * RS232 to LIN
 */
void RS232_To_LIN(uint8_t* pRS232Buff)
{
	LIN_Send_Flag = DISABLE;
	uint8_t index = 0;
	chip_Num = pRS232RxBuff[0];
	EXV_Test_Step = (pRS232RxBuff[1] << 8) | pRS232RxBuff[2];
	pLINTxBuff[index++] = chip[chip_Num].write_PID;
	pLINTxBuff[index++] = pRS232RxBuff[2];
	pLINTxBuff[index++] = pRS232RxBuff[1];
	pLINTxBuff[index++] = chip[chip_Num].EXV_Move_Enable;
	pLINTxBuff[index++] = chip[chip_Num].EXV_Init_Request;
	//剩余的字节数有0xFF填充
	while(index < LIN_TX_MAXSIZE - 1)
	{
		pLINTxBuff[index++] = 0xFF;
	}
	LIN_Send_Flag = ENABLE;
}

/**
 * 发送LIN数据，包括读取帧和写帧
 */
void Send_LIN_Data()
{
	if(LIN_Send_Flag)
	{
		LIN_Tx_PID_Data(&huart2,pLINTxBuff,LIN_TX_MAXSIZE - 1,LIN_CK_ENHANCED);
		LIN_Send_Flag = DISABLE;
		LIN_Read_Flag = ENABLE;
	}
	if(LIN_Read_Flag)
	{
		LIN_Tx_PID(&huart2, chip[chip_Num].read_PID);
		HAL_Delay(200);
	}
}

/**
 * 设置响应给RS232上位机的数据
 */
void Send_Resp_Data(uint8_t* pBuff,uint16_t data)
{
	*pBuff = data >> 8;
	*(pBuff + 1) = data;
	HAL_UART_Transmit(&huart1,pBuff,sizeof(data),HAL_MAX_DELAY);
	//读取标志位置为不发送读取数据帧
	LIN_Read_Flag = DISABLE;
	//重置重试的次数为3
	retries = 5;
	//发送响应数据后表示本次测试结束，清空发送数据缓存
	memset(pLINTxBuff,0,LIN_TX_MAXSIZE);
    //清除芯片编号
    chip_Num = 0;
}

/**
 * 数据处理函数
 */
void LIN_Data_Process()
{
	//响应数组
	uint8_t RS232_Resp_Result[2] = {0};
	//电机转动步长
	uint16_t EXV_Run_Step = 0;
	//通过校验位-校验数据
	uint8_t ckm = 0;
	//pLINRxBuff + 2表示从接收的第3个数据开始，因为接收数组第1个是同步间隔段，第2个是同步段（0x55）
	ckm = LIN_Check_Sum_En(pLINRxBuff + 2,LIN_CHECK_EN_NUM);
	//如果校验不通过，丢弃这帧数据
	if(ckm != pLINRxBuff[LIN_RX_MAXSIZE - 1])
	{
		return;
	}
	//解析数据具有优先级：LIN通信故障->电机故障->电压异常->温度异常->电机停止标志->判断步长
	//校验LIN通信故障反馈
	if((pLINRxBuff[3] & EXV_F_RESP_COMP) == EXV_F_RESP_ERROR)
	{
		Send_Resp_Data(RS232_Resp_Result,RS232_RESP_LIN_COMM_ERROR);
	}
	//校验故障状态
	else if((pLINRxBuff[4] & EXV_ST_FAULT_COMP) > 0)
	{
		uint8_t fault_index = pLINRxBuff[4] & EXV_ST_FAULT_COMP;
		switch(fault_index)
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
	//校验电压状态
	else if((pLINRxBuff[4] & EXV_ST_VOLTAGE_COMP) > 0)
	{
		uint8_t voltage_index = pLINRxBuff[4] & EXV_ST_VOLTAGE_COMP;
		switch(voltage_index)
		{
		case EXV_ST_VOLTAGE_OVER:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OVER_VOLTAGE);
			break;
		case EXV_ST_VOLTAGE_UNDER:
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_UNDER_VOLTAGE);
			break;
		}
	}
	//校验温度状态
	else if((pLINRxBuff[4] & EXV_OVERTEMP_COMP) == EXV_OVERTEMP_OVER)
	{
		Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OVERTEMP);
	}
	//电机停止转动
	else if((pLINRxBuff[3] & EXV_ST_RUN_COMP) == EXV_ST_RUN_NOT_MOVE)
	{
		//计算电机转动步长，步长低字节在前高字节在后
		EXV_Run_Step = (pLINRxBuff[6] << 8) | pLINRxBuff[5];
		if(EXV_Run_Step == EXV_Test_Step)
		{
			Send_Resp_Data(RS232_Resp_Result,RS232_RESP_OK);
		}
		//重试3次发送电机运动使能
		else
		{
			LIN_Send_Flag = ENABLE;
			retries--;
			//当3次电机运动使能后，电机转动步长与测试步长不一致，发送错误信息
			if(retries <= 0)
			{
				Send_Resp_Data(RS232_Resp_Result,RS232_RESP_ERROR);
				//发送标志置为不发送写数据帧
				LIN_Send_Flag = DISABLE;
			}
		}
	}
	//这帧数据解析完成，清空接收缓存数据
	memset(pLINRxBuff,0,LIN_RX_MAXSIZE);
}
