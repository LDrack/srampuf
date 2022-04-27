/**
  ******************************************************************************
  * @file    stm32f401re_system.h
  * @author  Lucas Drack
  * @version V1.0
  * @date    2022-03-21
  * @brief   Control header file for STM Nucleo Board with STM32F401RE.
  ******************************************************************************
  */

#ifndef __NUCLEO_F401RE_SYSTEM_H
#define __NUCLEO_F401RE_SYSTEM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void);

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 84000000
  *            HCLK(Hz)                       = 84000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 336
  *            PLL_P                          = 4
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale2 mode
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void);

/**
  * @brief  Get unique ID of device in 32-bit (byte) read mode. Unique ID is 96bit long,
  *         so it has to be read in parts.
  * @param  nr: Value beteen 0 and 2, depending on which 32-bit part of the ID is desired.
  * @retval 32-bit part of unique device ID.
  */
uint32_t DeviceID_Get32(int nr);



#endif