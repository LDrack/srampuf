#include "peripherals.h"

// See stm32f401re datasheet p. 112 for calibration register addresses
#define TEMP30_CAL_VALUE 			((uint16_t*)((uint32_t)0x1FFF7A2C))
#define TEMP110_CAL_VALUE 			((uint16_t*)((uint32_t)0x1FFF7A2E))
#define TEMP30 						30.0f
#define TEMP110 					110.0f


void UserButton_Init(void) {
    BTN_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef BTN_InitStruct;
	
	BTN_InitStruct.Pin = BTN_PIN;
	BTN_InitStruct.Mode = GPIO_MODE_INPUT;
	BTN_InitStruct.Pull = GPIO_NOPULL;
	BTN_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BTN_GPIO_PORT, &BTN_InitStruct);
}


int Read_UserButton(void) {
	return HAL_GPIO_ReadPin(BTN_GPIO_PORT, BTN_PIN);
}


void UserLed_Init(void) {
	LED_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef LED_InitStruct;
	
	LED_InitStruct.Pin = LED_PIN;
	LED_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	LED_InitStruct.Pull = GPIO_PULLUP;
	LED_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED_GPIO_PORT, &LED_InitStruct);
}


void UserLed_Off(void) {
	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET);
}


void UserLed_On(void) {
	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);
}


void usermode_Error(int err) {
	while (err != 0) {
		if(HAL_GPIO_ReadPin(LED_GPIO_PORT, LED_PIN) == GPIO_PIN_RESET) {
			UserLed_On();
		} else {
			UserLed_Off();
		}
		
		HAL_Delay(1000);
	}
}


int UART_Init(USART_HandleTypeDef *husart)
{
	/* --------------------------- System Clocks Configuration -----------------*/
	USART2_CLK_ENABLE();
	USART2_GPIO_CLK_ENABLE();


	/*-------------------------- GPIO Configuration ----------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.Pin = USART2_PIN; // PA.2 USART2_TX, PA.3 USART2_RX
	GPIO_InitStructure.Alternate = GPIO_AF7_USART2;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USART2_PORT, &GPIO_InitStructure);


	/* USARTx configuration ------------------------------------------------------*/
	/* USARTx configured as follow:
	- BaudRate = 115200 baud
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Receive and transmit enabled
	*/
	husart->Instance = USART2;
	husart->Init.BaudRate = 115200;
	husart->Init.WordLength = USART_WORDLENGTH_8B;
	husart->Init.StopBits = USART_STOPBITS_1;
	husart->Init.Parity = USART_PARITY_NONE;
	husart->Init.Mode = USART_MODE_TX_RX;
	husart->Init.CLKPolarity = USART_POLARITY_LOW;
	husart->Init.CLKPhase = USART_PHASE_1EDGE;
	husart->Init.CLKLastBit = USART_LASTBIT_DISABLE;
	HAL_StatusTypeDef ret = HAL_USART_Init(husart);

	return (ret == HAL_OK ? 0 : ret);
}


int TempSensor_ADC_Init(ADC_HandleTypeDef *hadc)
{
	ADC_ChannelConfTypeDef sConfig;

    // Configure the global features of the ADC (Clock, Resolution, Data Alignment
	// and number of conversion)
	hadc->Instance = ADC1;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ScanConvMode = DISABLE;
	hadc->Init.ContinuousConvMode = DISABLE;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.NbrOfConversion = 1;
	hadc->Init.DMAContinuousRequests = DISABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(hadc) != HAL_OK)
	{
	  return -1;
	}

    // Configure for the selected ADC regular channel its corresponding
	// rank in the sequencer and its sample time.
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
	{
	  return -2;
	}

	return 0;
}

// Called from HAL
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hadc->Instance==ADC1)
	{
		/* ADC1 clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration */
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}


float TempSensor_Read(ADC_HandleTypeDef *hadc)
{
	float temperature = 0.0;
	float sensorValue = 0.0;
	float adcCalValue30 = (float)(*TEMP30_CAL_VALUE);
	float adcCalValue110 = (float)(*TEMP110_CAL_VALUE);
	HAL_StatusTypeDef ret;

	// Disable Vbat signal from input channel and wake up temp sensor from power down mode
	ADC->CCR |= ADC_CCR_TSVREFE;
	ADC->CCR &= ~ADC_CCR_VBATE ;

	ret = HAL_ADC_Start(hadc);
	if(ret == HAL_OK) {
		temperature = (float)((TEMP110 - TEMP30) / (adcCalValue110 - adcCalValue30) * (sensorValue - adcCalValue30) + TEMP30);
	}
	ret = HAL_ADC_PollForConversion(hadc, 1000);
	if(ret == HAL_OK)
	{
		sensorValue = (float)HAL_ADC_GetValue(hadc);
		HAL_ADC_Stop(hadc);
		temperature = (float)((TEMP110 - TEMP30) / (adcCalValue110 - adcCalValue30) * (sensorValue - adcCalValue30) + TEMP30);
	}
	else
	{
		temperature = -273;
	}

	return temperature;
}






