/*
 * lin_usart1.h
 *
 *  Created on: Apr 8, 2023
 *      Author: 陈骏骏
 */

#ifndef INC_LIN_USART1_H_
#define INC_LIN_USART1_H_

#include "main.h"

//1个同步字节（0x55） + PID（1个字节） + （数据帧 （8个字节）  +  校验和（1个字节））
#define LIN_RX_MAXSIZE 11
//PID（1个字节） + （数据帧 （8个字节）  +  校验和（1个字节））
#define LIN_TX_MAXSIZE 10

extern uint8_t pLINRxBuff[LIN_RX_MAXSIZE];
extern uint8_t pLINTxBuff[LIN_TX_MAXSIZE];
//当前测试的电机步长
extern uint16_t EXV_Test_Step;

//LIN校验模式
typedef enum
{
	//标准校验
	LIN_CK_STANDARD = 0x00U,
	//增强校验
	LIN_CK_ENHANCED = 0x01U
}LIN_CK_Mode;

//LIN_PID+P0+P1,LSB,PID列表
typedef enum
{
	LIN_PID_00_0x00 = 0x80,
	LIN_PID_01_0x01 = 0xC1,
	LIN_PID_02_0x02 = 0x42,
	LIN_PID_03_0x03 = 0x03,
	LIN_PID_04_0x04 = 0xC4,
	LIN_PID_05_0x05 = 0x85,
	LIN_PID_06_0x06 = 0x06,
	LIN_PID_07_0x07 = 0x47,
	LIN_PID_08_0x08 = 0x08,
	LIN_PID_09_0x09 = 0x49,
	LIN_PID_10_0x0A = 0xCA,
	LIN_PID_11_0x0B = 0x8B,
	LIN_PID_12_0x0C = 0x4C,
	LIN_PID_13_0x0D = 0x0D,
	LIN_PID_14_0x0E = 0x8E,
	LIN_PID_15_0x0F = 0xCF,
	LIN_PID_16_0x10 = 0x50,
	LIN_PID_17_0x11 = 0x11,
	LIN_PID_18_0x12 = 0x92,
	LIN_PID_19_0x13 = 0xD3,
	LIN_PID_20_0x14 = 0x14,
	LIN_PID_21_0x15 = 0x55,
	LIN_PID_22_0x16 = 0xD6,
	LIN_PID_23_0x17 = 0x97,
	LIN_PID_24_0x18 = 0xD8,
	LIN_PID_25_0x19 = 0x99,
	LIN_PID_26_0x1A = 0x1A,
	LIN_PID_27_0x1B = 0x5B,
	LIN_PID_28_0x1C = 0x9C,
	LIN_PID_29_0x1D = 0xDD,
	LIN_PID_30_0x1E = 0x5E,
	LIN_PID_31_0x1F = 0x1F,
	LIN_PID_32_0x20 = 0x20,
	LIN_PID_33_0x21 = 0x61,
	LIN_PID_34_0x22 = 0xE2,
	LIN_PID_35_0x23 = 0xA3,
	LIN_PID_36_0x24 = 0x64,
	LIN_PID_37_0x25 = 0x25,
	LIN_PID_38_0x26 = 0xA6,
	LIN_PID_39_0x27 = 0xE7,
	LIN_PID_40_0x28 = 0xA8,
	LIN_PID_41_0x29 = 0xE9,
	LIN_PID_42_0x2A = 0x6A,
	LIN_PID_43_0x2B = 0x2B,
	LIN_PID_44_0x2C = 0xEC,
	LIN_PID_45_0x2D = 0xAD,
	LIN_PID_46_0x2E = 0x2E,
	LIN_PID_47_0x2F = 0x6F,
	LIN_PID_48_0x30 = 0xF0,
	LIN_PID_49_0x31 = 0xB1,
	LIN_PID_50_0x32 = 0x32,
	LIN_PID_51_0x33 = 0x73,
	LIN_PID_52_0x34 = 0xB4,
	LIN_PID_53_0x35 = 0xF5,
	LIN_PID_54_0x36 = 0x76,
	LIN_PID_55_0x37 = 0x37,
	LIN_PID_56_0x38 = 0x78,
	LIN_PID_57_0x39 = 0x39,
	LIN_PID_58_0x3A = 0xBA,
	LIN_PID_59_0x3B = 0xFB,
	LIN_PID_60_0x3C = 0x3C,
	LIN_PID_61_0x3D = 0x7D,
	LIN_PID_62_0x3E = 0xFE,
	LIN_PID_63_0x3F = 0xBF
} LIN_PID_List;

//电机使能
typedef enum
{
	EXV_MOVE_IGNORE = 0x00,
	EXV_MOVE_CMD = 0x01
}EXV_Move_Enable;

//初始化请求
typedef enum
{
	EXV_INIT_NO_REQ = 0x00,
	EXV_INIT_START = 0x01,
	EXV_INIT_WAIT_FINISH = 0x02
}EXV_Init_Request;

typedef enum
{
	EXV_F_RESP_NO_ERROR = 0x00,
	EXV_F_RESP_ERROR = 0x01
}EXV_F_Response;

typedef enum
{
	EXV_ST_INIT_NOT = 0x00,
	EXV_ST_INIT_PROCESS = 0x01,
	EXV_ST_INIT_SUCCESS = 0x02
}EXV_St_Initial;

typedef enum
{
	EXV_ST_RUN_NOT_MOVE = 0x00,
	EXV_ST_RUN_MOVING = 0x01
}EXV_St_Running;

typedef enum
{
	EXV_ST_FAULT_NOT = 0x00,
	EXV_ST_FAULT_SHORTED = 0x01,
	EXV_ST_FAULT_OPENLOAD = 0x02,
	EXV_ST_FAULT_OVERTEMP = 0x03,
	EXV_ST_FAULT_ACTUATORFAULT = 0x04
}EXV_St_Fault;

typedef enum
{
	EXV_ST_VOLTAGE_OK = 0x00,
	EXV_ST_VOLTAGE_OVER = 0x01,
	EXV_ST_VOLTAGE_UNDER = 0x02
}EXV_St_Voltage;

typedef enum
{
	EXV_OVERTEMP_OK = 0x00,
	EXV_OVERTEMP_OVER = 0x01
}EXV_W_OverTemp;

void LIN_Tx_PID_Data(UART_HandleTypeDef *huart, uint8_t *buf, uint8_t lens, LIN_CK_Mode CK_Mode);
void LIN_Tx_PID(UART_HandleTypeDef *huart, uint8_t PID);
void LIN_Data_Process();

#endif /* INC_LIN_USART1_H_ */
