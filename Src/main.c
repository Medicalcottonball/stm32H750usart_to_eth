/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "lwip.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "iwdg.h"
#include "24cxx.h"
#include "delay.h"
#include "usart.h"
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
uint8_t startSample = 0;
uint16_t time100us = 0;
uint16_t sampleCnt = 0;
uint8_t laserSta = 0;
uint16_t sampleNumber = 500;
struct tcp_pcb *tcppcb;  	
struct ip_addr rmtipaddr; 
struct tcp_pcb *tcppcbnew;  	
struct tcp_pcb *tcppcbconn;  
uint8_t g_config_way = 0;// 0:uasrt 1:net 
uint32_t g_y1 = 0;
uint32_t g_y2 = 0;	
uint16_t tcp_send_len = 0;
uint8_t tcp_send_flag = 0;
int tcp_send_cnt = 0;
int tcp_send_number = 0;
int tcp_send_type = 0;
u16 led_num = 0;
uint8_t devicepaddr=0;
uint8_t samplepaddr=0;
uint8_t peakpaddr=0;
uint8_t reffiberaddr=70;
uint8_t tempaddr=80;
uint8_t coefaddr=100;
uint8_t calparamaddr=120;

uint8_t Senbuff[] = "Hello ";  //定义数据发送数组

uint8_t TX_Buffer[1024];
uint8_t Rx_Buffer[1024]; //接收数据20个字符
uint8_t len_total;
uint8_t flag_rev_finish = 0;
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 

#define REC_LENGTH  1024		//定义接收数据长度，根据使用需求自定义
uint8_t UART1_Rx_Buf[REC_LENGTH];	//接收数据存储
uint8_t UART1_Rx_flg=0;		//定时器中断标志位
uint8_t UART1_Rx_cnt=0;		//串口接收数据下标
uint8_t Rxbuff[1]; //串口接收数据缓冲


//uint32_t test[32] __attribute__((section(".ARM.__at_0x24000000")))= {0};
Config_info_t G_devConfig = {
	.versionName = VERSION_NAME,
	.versionCode = VERSION_CODE,
	.sn = DEV_SN,
	.addr = 1,
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
void RS232_TO_TCP(void);
	

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)		//定义 USART1 接收完成回调函数功能
{
//	if(huart->Instance==USART1)
//	  {
	//	__HAL_TIM_SET_COUNTER(&htim7,0);//定时器清
		  /* Read data from the RX data register */
			//	HAL_UART_Receive_IT(&huart1,(uint8_t *)Rxbuff,1);//每接收一个数据，就打1次串口中断接收，否则只会接收1个数据就停止接收
	           if(0 == UART1_Rx_cnt)
	           {
	        	   __HAL_TIM_CLEAR_FLAG(&htim3,TIM_FLAG_UPDATE);
	        	   HAL_TIM_Base_Start_IT(&htim3);//??启定??
	           }
		__HAL_TIM_SET_COUNTER(&htim3,0);//清空计数??


	        	  UART1_Rx_Buf[UART1_Rx_cnt] = Rxbuff[0];
	        	   UART1_Rx_cnt++;
	        	   HAL_UART_Receive_IT(&huart1,Rxbuff,1);

}



int main(void)
{
	err_t err;
	uint8_t res=0;	
	samplepaddr=sizeof(G_devConfig)+5;
	
  MPU_Config();
  SCB_EnableDCache();
  HAL_Init();

  SCB_EnableICache();
  SCB_EnableDCache();
	SCB->CACR|=1<<2;   
  SystemClock_Config();

  PeriphCommonClock_Config();

  MX_GPIO_Init();  
  MX_DMA_Init();
  MX_LWIP_Init();
	MX_USART1_UART_Init();
  MX_IWDG1_Init();

	
	delay_init(480);

  MX_TIM2_Init();
  MX_TIM3_Init();
  HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	 #if 0
   MPU_Set_Protection(
      0x24000000,
      MPU_REGION_SIZE_512KB,
      MPU_REGION_NUMBER2,
      MPU_REGION_NO_ACCESS,
      MPU_REGION_FULL_ACCESS,
      MPU_INSTRUCTION_ACCESS_ENABLE,
      MPU_ACCESS_NOT_CACHEABLE,
      MPU_ACCESS_BUFFERABLE);

			MPU_Set_Protection(
			0x30040000,
			MPU_REGION_SIZE_256B,
			MPU_REGION_NUMBER1,
			MPU_REGION_NO_ACCESS,
			MPU_REGION_FULL_ACCESS,
			MPU_INSTRUCTION_ACCESS_ENABLE,
			MPU_ACCESS_NOT_CACHEABLE,
			MPU_ACCESS_BUFFERABLE);

			MPU_Set_Protection(
			0x30044000,
			MPU_REGION_SIZE_16KB,
			MPU_REGION_NUMBER2,
			MPU_REGION_NO_ACCESS,
			MPU_REGION_FULL_ACCESS,
			MPU_INSTRUCTION_ACCESS_ENABLE,
			MPU_ACCESS_CACHEABLE,
			MPU_ACCESS_NOT_BUFFERABLE);
#endif
 

	tcppcbnew=tcp_new();	
	if(tcppcbnew)			
	{ 
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);
		if(err==ERR_OK)	
		{
			tcppcbconn=tcp_listen(tcppcbnew); 		
			tcp_accept(tcppcbconn,tcp_server_accept); 	
		}else res=1;  
	}else res=1;
	if(res)
	{
			NVIC_SystemReset();
	}
		
	//HAL_UART_Receive_DMA(&huart1,Rx_Buffer,20);
	//__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)Rxbuff,1);//打开串口中断接收
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_IWDG_Refresh(&hiwdg1);
		//RS232_TO_TCP();
		HAL_Delay(1);
		//tcp_server_test();
    MX_LWIP_Process();
		
		if(UART1_Rx_flg)//表示接收完成或超???
	     {
	       //HAL_UART_Transmit(&huart1,UART1_Rx_Buf,UART1_Rx_cnt,0xffff);
				 struct tcp_pcb *cpcb;
				 for(cpcb = tcp_active_pcbs;cpcb != NULL; cpcb = cpcb->next)
				{
						cpcb -> flags = TF_NODELAY;
            tcp_write(cpcb,UART1_Rx_Buf,UART1_Rx_cnt,TCP_WRITE_FLAG_COPY);
            tcp_output(cpcb);
					
        }
				 
	       for(int i = 0;i<UART1_Rx_cnt;i++)
	         UART1_Rx_Buf[i] = 0;
	       UART1_Rx_cnt = 0;
	       UART1_Rx_flg = 0;
	     }

  }

  /* USER CODE END 3 */
}



//串口收到数据通过tcp协议准发出去
void RS232_TO_TCP(void)
{
	
	struct tcp_pcb *cpcb;
	
	if(flag_rev_finish == 1)
	{
		flag_rev_finish = 0;
//		for(int i =0;i<1024;i++)
//      {
//        TX_Buffer[i]= Rx_Buffer[i];                                   //将接收的数据保存
//				
//      }

//			a++;
	
		for(cpcb = tcp_active_pcbs;cpcb != NULL; cpcb = cpcb->next)
        {
						cpcb -> flags = TF_NODELAY;
						HAL_UART_Transmit_DMA(&huart1,Rx_Buffer,len_total);
            tcp_write(cpcb,Rx_Buffer,len_total,TCP_WRITE_FLAG_COPY);
            tcp_output(cpcb);
					
        }
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	 if(htim==(&htim3))
	  {

	    UART1_Rx_flg = 1;
	    HAL_TIM_Base_Stop_IT(&htim3);//关闭定时
	  }
} 



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
 #if 0
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30044000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
/* USER CODE END 4 */

/* MPU Configuration */
#if 0
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30044000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
#endif
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
