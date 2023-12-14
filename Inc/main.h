/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define VERSION_NAME "v1.1"
#define VERSION_CODE 1
#define DEV_SN "202201"

typedef struct
{

	uint8_t versionName[20];

	uint16_t versionCode;
    
	uint8_t addr;
	
	uint8_t sn[20];

} Config_info_t;

#define CONFIG_SIZE sizeof(G_devConfig)
extern uint8_t startSample;
extern uint16_t time100us;
extern Config_info_t G_devConfig;
extern uint8_t laserSta;
extern uint16_t sampleNumber;
//extern uint32_t test[32] __attribute__((section(".ARM.__at_0x24000000")));
extern struct tcp_pcb *tcppcb;  	
extern struct ip_addr rmtipaddr; 
extern struct tcp_pcb *tcppcbnew;  	
extern struct tcp_pcb *tcppcbconn; 
extern uint8_t g_config_way;
extern uint32_t g_y1;
extern uint32_t g_y2;	
extern uint16_t tcp_send_len;
extern uint8_t tcp_send_flag;
extern int tcp_send_cnt;
extern int tcp_send_number;
extern int tcp_send_type;
extern uint8_t devicepaddr;
extern uint8_t samplepaddr;
extern uint8_t peakpaddr;
extern uint8_t reffiberaddr;
extern uint8_t tempaddr;
extern uint8_t coefaddr;
extern uint8_t calparamaddr;

extern TIM_HandleTypeDef htim3;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
