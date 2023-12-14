/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
#include "tim.h"
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include "tcp_server_demo.h" 
#include "stdio.h"
#include "string.h"  
#include "usart.h"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include "lwip.h"
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef heth;
//extern DMA_HandleTypeDef hdma_adc1;
//extern DMA_HandleTypeDef hdma_adc3;
//extern ADC_HandleTypeDef hadc1;
//extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
//extern DMA_HandleTypeDef hdma_uart4_rx;
//extern DMA_HandleTypeDef hdma_uart4_tx;
//extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim1;


extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;

extern uint8_t Rx_Buffer[20];
extern uint8_t TX_Buffer[20];
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 
extern struct tcp_pcb *tcppcbnew;  	
struct tcp_pcb *cpcb;

/* USER CODE BEGIN EV */
extern uint8_t len_total;
extern uint8_t flag_rev_finish;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	HAL_IncTick();
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  */


/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */


/**
  * @brief This function handles DMA1 stream4 global interrupt.
  */


/**
  * @brief This function handles ADC1 and ADC2 global interrupts.
  */

//以下新增代码========================================


/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
//	uint8_t TX_Buffer[] = "1232";
//	HAL_UART_Transmit_DMA(&huart1,TX_Buffer,sizeof(TX_Buffer));
  
	
//	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
//	{	
		
//		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
//		HAL_UART_DMAStop(&huart1);
//		len_total=20-__HAL_DMA_GET_COUNTER(huart1.hdmarx);
//		HAL_UART_Transmit_DMA(&huart1,Rx_Buffer,len_total);
//		for(tcppcbnew = tcp_active_pcbs;tcppcbnew != NULL; tcppcbnew = tcppcbnew->next)
//        {
//						tcppcbnew -> flags = TF_NODELAY;
//						HAL_UART_Transmit_DMA(&huart1,Rx_Buffer,len_total);
//            tcp_write(tcppcbnew,Rx_Buffer,len_total,TCP_WRITE_FLAG_COPY);
//            tcp_output(tcppcbnew);
//					
//        }

//		HAL_UART_Receive_DMA(&huart1,Rx_Buffer,20);
//		memset(Rx_Buffer,0,sizeof(Rx_Buffer));
		
		//memcpy(TX_Buffer,Rx_Buffer,1024);
		
		
		//HAL_UART_Transmit_DMA(&huart1,Rx_Buffer,len_total);
		//flag_rev_finish = 1;
		
//	}
	
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

//以上新增代码====================================


/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
	
  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
  
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
//void TIM3_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM3_IRQn 0 */

//  /* USER CODE END TIM3_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim3);
//  /* USER CODE BEGIN TIM3_IRQn 1 */
//  g_time_test++;
//  /* USER CODE END TIM3_IRQn 1 */
//}

/**
  * @brief This function handles UART4 global interrupt.
  */


/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */


/**
  * @brief This function handles Ethernet global interrupt.
  */
void ETH_IRQHandler(void)
{
  /* USER CODE BEGIN ETH_IRQn 0 */
  //ethernetif_input(&gnetif);
  //__HAL_ETH_DMA_CLEAR_IT(&heth, ETH_DMA_NORMAL_IT);
  //__HAL_ETH_DMA_CLEAR_IT(&heth, ETH_DMA_RX_IT);
  //__HAL_ETH_DMA_CLEAR_IT(&heth, ETH_DMA_TX_IT);
  /* USER CODE END ETH_IRQn 0 */
  HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */

  /* USER CODE END ETH_IRQn 1 */
}

/**
  * @brief This function handles ADC3 global interrupt.
  */


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
