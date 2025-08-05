/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "os.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef uint32_t  u32; 
typedef uint16_t u16; 
typedef uint8_t  u8; 
typedef int8_t  i8; 
typedef int16_t  i16; 
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern OS_ERR err;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define DHT11_Pin GPIO_PIN_4
#define DHT11_GPIO_Port GPIOC
#define Key_H1_Pin GPIO_PIN_0
#define Key_H1_GPIO_Port GPIOB
#define Key_H2_Pin GPIO_PIN_1
#define Key_H2_GPIO_Port GPIOB
#define Key_H3_Pin GPIO_PIN_12
#define Key_H3_GPIO_Port GPIOB
#define Key_H4_Pin GPIO_PIN_13
#define Key_H4_GPIO_Port GPIOB
#define Key_L1_Pin GPIO_PIN_14
#define Key_L1_GPIO_Port GPIOB
#define Key_L2_Pin GPIO_PIN_15
#define Key_L2_GPIO_Port GPIOB
#define Key_L3_Pin GPIO_PIN_6
#define Key_L3_GPIO_Port GPIOC
#define Key_L4_Pin GPIO_PIN_7
#define Key_L4_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_11
#define BEEP_GPIO_Port GPIOA
#define RELAY_Pin GPIO_PIN_12
#define RELAY_GPIO_Port GPIOA
#define SMG_DATA_Pin GPIO_PIN_15
#define SMG_DATA_GPIO_Port GPIOA
#define SMG_LCLK_Pin GPIO_PIN_3
#define SMG_LCLK_GPIO_Port GPIOB
#define SMG_SCLK_Pin GPIO_PIN_4
#define SMG_SCLK_GPIO_Port GPIOB
#define SMG_w3_Pin GPIO_PIN_5
#define SMG_w3_GPIO_Port GPIOB
#define SMG_w2_Pin GPIO_PIN_6
#define SMG_w2_GPIO_Port GPIOB
#define SMG_w1_Pin GPIO_PIN_7
#define SMG_w1_GPIO_Port GPIOB
#define Oled_SCL_Pin GPIO_PIN_8
#define Oled_SCL_GPIO_Port GPIOB
#define Oled_SDA_Pin GPIO_PIN_9
#define Oled_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define wLED0(x) HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rLED0 HAL_GPIO_ReadPin(LED0_GPIO_Port, LED0_Pin)
#define wLED1(x) HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rLED1 HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin)
#define wLED2(x) HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rLED2 HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin)
#define wLED3(x) HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rLED3 HAL_GPIO_ReadPin(LED3_GPIO_Port, LED3_Pin)
#define wBeep(x) HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rBeep HAL_GPIO_ReadPin(BEEP_GPIO_Port, BEEP_Pin)
#define wRelay(x) HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define rRelay HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
