/**
  ******************************************************************************
  * @file    peripherals.h
  * @author  Lucas Drack
  * @version V1.0
  * @date    2022-03-16
  * @brief   Control header file for STM Nucleo Board with STM32F401RE.
  ******************************************************************************
  */

#ifndef __NUCLEO_F401RE_UTILS_H
#define __NUCLEO_F401RE_UTILS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
#define LED_PIN                                GPIO_PIN_5
#define LED_GPIO_PORT                          GPIOA
#define LED_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()

#define BTN_PIN                                GPIO_PIN_13
#define BTN_GPIO_PORT                          GPIOC
#define BTN_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()

#define USART2_PIN                             GPIO_PIN_2 | GPIO_PIN_3
#define USART2_PORT                            GPIOA
#define USART2_CLK_ENABLE()                    __HAL_RCC_USART2_CLK_ENABLE()
#define USART2_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialize buttons including GPIO ports.
  * @param  None
  * @retval None
*/
void UserButton_Init(void);

/**
  * @brief  Read user button.
  * @param  None
  * @retval GPIO_PIN_RESET when button is pressed, GPIO_PIN_SET when NOT pressed.
*/
int Read_UserButton(void);

/**
  * @brief  Initialize led including GPIO ports.
  * @param  None
  * @retval None
*/
void UserLed_Init(void);

/**
  * @brief  Turn off user LED.
  * @param  None
  * @retval None
*/
void UserLed_Off(void);

/**
  * @brief  Turn on user LED.
  * @param  None
  * @retval None
*/
void UserLed_On(void);

/**
  * @brief  Enter error loop. Turns on user LED and while(1)
  * @param  err: pass any value != 0. This allows to find from where usermode_Error() was called.
  * @retval None
*/
void usermode_Error(int err);

/**
  * @brief  Initialize led including GPIO ports.
  * @param  None
  * @retval 0 on success, error code on error.
*/
int UART_Init(USART_HandleTypeDef *husart);

/**
  * @brief  Initialize ADC to be used with temperature sensor.
  * @param  None
  * @retval 0 on success, error code on error.
*/
int TempSensor_ADC_Init(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);

/**
  * @brief  Read temperature sensor.
  * @param  None
  * @retval Temperature in °C.
*/
float TempSensor_Read(ADC_HandleTypeDef *hadc);


#endif