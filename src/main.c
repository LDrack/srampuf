#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stm32f401re_system.h>
#include <peripherals.h>
#include "srampuf.h"



USART_HandleTypeDef husart = {0};
ADC_HandleTypeDef hadc = {0};



// Attention: use initSramPuf() first before calling these functions!
void printUniqueID() {
	int ret = uartPrintf("Unique ID: - 0x%08X 0x%08X 0x%08X -\n",
					 DeviceID_Get32(0), DeviceID_Get32(1), DeviceID_Get32(2));
	if (ret < 0) usermode_Error(__LINE__);
}

void printTemperature() {
	float temperature = 0.0;
	temperature = TempSensor_Read(&hadc);
	int ret = uartPrintf("Temperature (rounded): %d °C\n", (int)temperature);
	if (ret < 0) usermode_Error(__LINE__);
}

void sendPacket() {
	const int len = getSrampufProtocolBytes();
	unsigned char buf[len];
	float temperature = TempSensor_Read(&hadc);
	int ret = srampufProtocol(buf, len, BS_NUCLEOF401RE, temperature);
	if (ret < 0) usermode_Error(__LINE__);
	if(HAL_USART_Transmit(&husart, (uint8_t *)buf, len, HAL_MAX_DELAY) != HAL_OK) usermode_Error(__LINE__);
}


void sendCompletePufs() {
	const int nPufs = 35;
	
	unsigned int pufBegin = 0x20005000;
	const unsigned int pufSize  = 0x800;
	for (int i = 0; i < nPufs; i++)
	{
		unsigned int pufEnd = pufBegin + pufSize;
		if (initSramPuf(&husart, pufBegin, pufEnd) != 0) usermode_Error(__LINE__);
		sendPacket();
		pufBegin = pufEnd;

		HAL_Delay(2000);
	}
}


// Debounces button using a shift register method. Use in loop.
// When button was pressed and is let go, this function returns 1, otherwise 0.
int onceOnButtonPress(void) {
	static uint16_t buttonDebounce = 0;

	uint16_t readButton = (Read_UserButton() == GPIO_PIN_RESET) ? 1 : 0;
	buttonDebounce = (buttonDebounce << 1) | readButton | 0xe000;

	if (buttonDebounce==0xf000) { // High for 1 bits low for 12 (each bit is 1 loop through this code).
	   return 1;
	}
	return 0;
}



int main(void)
{
	// int ret;

	SystemClock_Config();
	HAL_Init();

	UserButton_Init();
	UserLed_Init();

	if (UART_Init(&husart)) usermode_Error(__LINE__);
	// if (initSramPuf(&husart, 0x20015000, 0x20015100) != 0) usermode_Error(__LINE__);
	// if (initSramPuf(&husart, 0x20005000, 0x20006388) != 0) usermode_Error(__LINE__);
	if (TempSensor_ADC_Init(&hadc)) usermode_Error(__LINE__);

	// char *msg = "UART is MUART\n";
	// HAL_USART_Transmit(&husart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

	// f401reMemDump();

	// srampufDump(1);
	// srampufDump(0);

	
	

	

	while (1)
	{
		if(Read_UserButton() == GPIO_PIN_RESET) {
			UserLed_On();
		} else {
			UserLed_Off();
		}

		if(onceOnButtonPress()) {
			// sendPacket();
			sendCompletePufs();
		}
		
		HAL_Delay(50);
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}





