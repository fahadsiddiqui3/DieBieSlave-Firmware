#include "stm32f3xx_hal.h"
#include "usb_device.h"
#include "modDelay.h"
#include "modEffect.h"
#include "middleSOES.h"
#include "middleNunChuck.h"

void SystemClock_Config(void);
void Error_Handler(void);

middleNunChuckDataStruct mainNunChuckSensorDataStruct;

void newNunChuckDataEventHandler(middleNunChuckDataStruct newData);
void newSOESReadBufferUpdateHandler(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
	
	modDelayInit();
	modEffectInit();
	middleSOESInit();
	middleSOESReadBufferUpdateEvent(&newSOESReadBufferUpdateHandler);
	
	middleNunChuckInit();
	middleNunChuckNewDataEvent(&newNunChuckDataEventHandler);
	
  while(true) {
		modEffectTask();
		middleSOESTask();
		middleNunChuckTask();
  }
}

void newNunChuckDataEventHandler(middleNunChuckDataStruct newData) {
	memcpy(&mainNunChuckSensorDataStruct,&newData,sizeof(middleNunChuckDataStruct));
}

void newSOESReadBufferUpdateHandler(void) {
	// Update the SOES readbuffer
	middleSOESReadBuffer.NunChuck.JoyStickX = mainNunChuckSensorDataStruct.joystickX;
	middleSOESReadBuffer.NunChuck.JoyStickY = mainNunChuckSensorDataStruct.joystickY;
	
	middleSOESReadBuffer.NunChuck.AcceleroMeterX = mainNunChuckSensorDataStruct.accelerometerX;
	middleSOESReadBuffer.NunChuck.AcceleroMeterY = mainNunChuckSensorDataStruct.accelerometerY;
	middleSOESReadBuffer.NunChuck.AcceleroMeterZ = mainNunChuckSensorDataStruct.accelerometerZ;
	
	middleSOESReadBuffer.NunChuck.ButtonC = mainNunChuckSensorDataStruct.buttonC;
	middleSOESReadBuffer.NunChuck.ButtonZ = mainNunChuckSensorDataStruct.buttonZ;
	
	middleSOESReadBuffer.NunChuck.NunChuckDataValid = mainNunChuckSensorDataStruct.dataValid;
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_TIM2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_SYSCLK;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_PLLCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void Error_Handler(void) {
  while(1);
}
