/*
 * rs485_usart3.c
 *
 *  Created on: 2023年4月10日
 *      Author: 陈骏骏
 */


#include "rs485_usart3.h"

//RS485接收数据缓存
uint8_t pRS485RxBuff[RS485_MAXSIZE];

/**
 * CRC校验
 * 形参：_pBuf，参与校验的数据
 * 		usLen，数据长度
 * 返回值：16位整数值，此结果高字节先传送，低字节后传送
 */
uint16_t Cal_CRC16(uint8_t *pBuf,uint8_t lens)
{
	uint8_t CRC_H = 0xFF;		//高CRC字节初始化
	uint8_t CRC_L = 0xFF;		//低CRC字节初始化
	uint16_t usIndex;			//CRC循环中的索引

	while(lens--)
	{
		usIndex = CRC_L ^ *(pBuf++);	//计算CRC值
		CRC_L = CRC_L ^ s_CRC_H[usIndex];
		CRC_L = s_CRC_L[usIndex];
	}
	return ((uint16_t)CRC_H << 8 | CRC_L);
}

/**
 * 读取瞬时流量
 * 寄存器地址：0x003A,0x003B
 * 功能码：03
 */
void read_flow(void)
{
	//RS485发送数据缓存
	uint8_t pTxBuff[RS485_MAXSIZE];
	uint8_t index = 0;
	uint16_t num = 0x0002;						//寄存器个数
	pTxBuff[index++] = device_address;			//设备地址
	pTxBuff[index++] = RS485_FC_WRITE;			//功能码
	pTxBuff[index++] = RS485_RA_0X003A >> 8;	//寄存器高位字节
	pTxBuff[index++] = RS485_RA_0X003A;			//寄存器低位字节
	pTxBuff[index++] = num >> 8;				//寄存器个数 高位
	pTxBuff[index++] = num;						//寄存器个数 低位
	uint16_t crc = Cal_CRC16(pTxBuff, index);	//计算CRC值
	pTxBuff[index++] = crc >> 8;				//CRC高位
	pTxBuff[index++] = crc;						//CRC低位
	//发送数据
	//HAL_UART_Transmit(&huart3, pTxBuff, index, HAL_MAX_DELAY);
}

/**
 * 关闭写保护
 * 寄存器地址：0x00FF
 * 功能码：06
 * 数据：0xAA55
 * 形参：_buf，数据缓冲区
 * 		lens，数据长度
 */
void close_write_FF(void)
{
	//RS485发送数据缓存
	uint8_t pTxBuff[RS485_MAXSIZE];
	uint16_t send_data = 0xAA55;
	uint8_t index = 0;
	pTxBuff[index++] = device_address;			//设备地址
	pTxBuff[index++] = RS485_FC_WRITE;			//功能码
	pTxBuff[index++] = RS485_RA_0X00FF >> 8;	//寄存器高位字节
	pTxBuff[index++] = RS485_RA_0X00FF;			//寄存器低位字节
	pTxBuff[index++] = send_data >> 8;			//寄存器值 高位
	pTxBuff[index++] = send_data;				//寄存器值 低位
	uint16_t crc = Cal_CRC16(pTxBuff, index);	//计算CRC值
	pTxBuff[index++] = crc >> 8;				//CRC高位
	pTxBuff[index++] = crc;						//CRC低位
	//发送数据
	//HAL_UART_Transmit(&huart3, pTxBuff, index, HAL_MAX_DELAY);
}

/**
 * 关闭阀门
 * 寄存器地址：0x00F5
 * 功能码：06
 * 数据：0x0001
 */
void close_valve(void)
{
	close_write_FF();
	//RS485发送数据缓存
	uint8_t pTxBuff[RS485_MAXSIZE];
	uint16_t send_data = 0x0001;
	uint8_t index = 0;
	pTxBuff[index++] = device_address;			//设备地址
	pTxBuff[index++] = RS485_FC_WRITE;			//功能码
	pTxBuff[index++] = RS485_RA_0X00F5 >> 8;	//寄存器高位字节
	pTxBuff[index++] = RS485_RA_0X00F5;			//寄存器低位字节
	pTxBuff[index++] = send_data >> 8;			//寄存器值 高位
	pTxBuff[index++] = send_data;				//寄存器值 低位
	uint16_t crc = Cal_CRC16(pTxBuff, index);	//计算CRC值
	pTxBuff[index++] = crc >> 8;				//CRC高位
	pTxBuff[index++] = crc;						//CRC低位
	//发送数据
//	HAL_UART_Transmit(&huart3, pTxBuff, index, HAL_MAX_DELAY);
}

/**
 * 打开阀门
 * 寄存器地址：0x00F5
 * 功能码：06
 * 数据：0x8001
 */
void open_valve(void)
{
	close_write_FF();
	//RS485发送数据缓存
	uint8_t pTxBuff[RS485_MAXSIZE];
	uint16_t send_data = 0x8001;
	uint8_t index = 0;
	pTxBuff[index++] = device_address;			//设备地址
	pTxBuff[index++] = RS485_FC_WRITE;			//功能码
	pTxBuff[index++] = RS485_RA_0X00F5 >> 8;	//寄存器高位字节
	pTxBuff[index++] = RS485_RA_0X00F5;			//寄存器低位字节
	pTxBuff[index++] = send_data >> 8;			//寄存器值 高位
	pTxBuff[index++] = send_data;				//寄存器值 低位
	uint16_t crc = Cal_CRC16(pTxBuff, index);	//计算CRC值
	pTxBuff[index++] = crc >> 8;				//CRC高位
	pTxBuff[index++] = crc;						//CRC低位
	//发送数据
//	HAL_UART_Transmit(&huart3, pTxBuff, index, HAL_MAX_DELAY);
}

/**
 * 自动校零
 * 寄存器地址：0x00F0
 * 功能码：06
 * 数据：0xAA55
 */
void auto_zero_setting()
{
	close_write_FF();
	//RS485发送数据缓存
	uint8_t pTxBuff[RS485_MAXSIZE];
	uint16_t send_data = 0xAA55;
	uint8_t index = 0;
	pTxBuff[index++] = device_address;			//设备地址
	pTxBuff[index++] = RS485_FC_WRITE;			//功能码
	pTxBuff[index++] = RS485_RA_0X00F0 >> 8;	//寄存器高位字节
	pTxBuff[index++] = RS485_RA_0X00F0;			//寄存器低位字节
	pTxBuff[index++] = send_data >> 8;			//寄存器值 高位
	pTxBuff[index++] = send_data;				//寄存器值 低位
	uint16_t crc = Cal_CRC16(pTxBuff, index);	//计算CRC值
	pTxBuff[index++] = crc >> 8;				//CRC高位
	pTxBuff[index++] = crc;						//CRC低位
	//发送数据
//	HAL_UART_Transmit(&huart3, pTxBuff, index, HAL_MAX_DELAY);
}

/**
 * 数据处理函数
 */
void RS485_Data_Process()
{
	HAL_UART_Transmit(&huart1, pRS485RxBuff, RS485_MAXSIZE, HAL_MAX_DELAY);
}
