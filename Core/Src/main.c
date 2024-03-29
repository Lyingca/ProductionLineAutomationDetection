/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lin_usart2.h"
#include "rs232_usart1.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t LINRevByte = 0;
uint8_t pLINRevByte = 0;
uint8_t LINRxFlag = 0;
uint8_t LINRxLength = 0;
uint8_t RS232RevByte = 0;
uint8_t pRS232RevByte = 0;
uint8_t RS232RxFlag = 0;
uint8_t RS232RxLength = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
    //解决断电后重新上电，程序运行异常的问题
    //原因：在程序刚开始时加上一个延时，因为外设上电时间不够，所以加个延时，等待外设上电，再进行初始化
//    HAL_Delay(700);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //清除串口空闲中断
  __HAL_UART_CLEAR_IDLEFLAG(&huart1);
  //打开串口空闲中断
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
  //开启中断接收
  Util_Receive_IT(&huart1);
  //一定要先清除串口空闲中断，然后在打开串口空闲中断，因为串口初始化完成后会自动将IDLE置位，
  // 导致还没有接受数据就进入到中断里面去了，所以打开IDLE之前，先把它清楚掉
  //清除串口空闲中断
  __HAL_UART_CLEAR_IDLEFLAG(&huart2);
  //打开串口空闲中断
  __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
  Util_Receive_IT(&huart2);
  //使能TJA1028LIN芯片的EN
  HAL_GPIO_WritePin(TJA1028_EN_GPIO_Port,TJA1028_EN_Pin,GPIO_PIN_SET);
  //使能TJA1028LIN芯片的RSTN
  HAL_GPIO_WritePin(TJA1028_RSTN_GPIO_Port,TJA1028_RSTN_Pin,GPIO_PIN_SET);

  //测试
//  pRS232RxBuff[0] = 0x00;
//  pRS232RxBuff[1] = 0x01;
//  pRS232RxBuff[2] = 0x4a;
//  RS232_To_LIN(pRS232RxBuff);
  //测试 end

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
      //循环发送数据
      Send_LIN_Data();
      if (LINRxFlag)
      {
          LIN_Data_Process(LINRxLength);
          LINRxFlag = 0;
      }
      if (RS232RxFlag && RS232RxLength == RS232_MAXSIZE && pRS232RxBuff[RS232_MAXSIZE - 1] == 0x0D)
      {
          HAL_UART_Transmit(&huart1, pRS232RxBuff, RS232_MAXSIZE, HAL_MAX_DELAY);
          RS232_To_LIN(pRS232RxBuff);
          memset(pRS232RxBuff,0,RS232_MAXSIZE);
          RS232RxFlag = 0;
      }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
 * 重写接收中断函数
 */
void Util_Receive_IT(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)
	{
        if(HAL_UART_Receive_IT(huart, &LINRevByte, 1) != HAL_OK)
        {
            Error_Handler();
        }
	}
	else if(huart == &huart1)
	{
		if(HAL_UART_Receive_IT(huart, &RS232RevByte, 1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

/**
 * 接收完成中断函数，用以在一帧数据接收完成时，对数据进行处理
 *
 * @brief Rx Transfer completed callback.
 * @param huart UART handle.
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//LIN协议
	if(huart == &huart2)
	{
        if (pLINRevByte < LIN_RX_MAXSIZE)
        {
            pLINRxBuff[pLINRevByte] = LINRevByte;
        }
        pLINRevByte++;
	}
	//RS232协议
	else if(huart == &huart1)
	{
        if (pRS232RevByte < RS232_MAXSIZE)
        {
            pRS232RxBuff[pRS232RevByte] = RS232RevByte;
        }
        pRS232RevByte++;
	}
	Util_Receive_IT(huart);
}

//串口空闲中断
void UART_IDLECallBack(UART_HandleTypeDef *huart)
{
    if(huart == &huart2)
    {
        if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart2);//清除标志位
            LINRxFlag = 1;
            LINRxLength = pLINRevByte;
            pLINRevByte = 0;
        }
    }
    else if (huart == &huart1)
    {
        if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);//清除标志位
            RS232RxFlag = 1;
            RS232RxLength = pRS232RevByte;
            pRS232RevByte = 0;
        }
    }
}

/**
 * 重写UART错误中断处理程序，重新开启USART中断
 *
 * @brief UART error callback.
 * @param huart UART handle.
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	//解决串口溢出，导致不断进入串口中断函数，使MCU过载的问题
	if(HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
	{
		//清除ORE标志位
		__HAL_UART_FLUSH_DRREGISTER(huart);
		Util_Receive_IT(huart);
		huart->ErrorCode = HAL_UART_ERROR_NONE;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
