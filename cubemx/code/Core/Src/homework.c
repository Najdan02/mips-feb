/*
 * homework.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "homework.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#include <stdlib.h>
#include <string.h>

#include "driver_lcd.h"
#include "driver_uart.h"
#include "driver_motor.h"
#include "driver_temp.h"

#include "keypad.h"
#include "gpio.h"

TimerHandle_t ledTimer;

static uint32_t tempValue;

static void homeworkTask(void *parameters)
{
	UNUSED(parameters);
	char messageTemp[9] = "Temp:   ";

	for (uint32_t i = 0; i < 8; i++) {
	    UART_AsyncTransmitCharacter(messageTemp[i]);
	}
	vTaskDelay(pdMS_TO_TICKS(50));  /* Yield so UART_TransmitTask (prio 6) can drain queue */

	while (1) {
		tempValue = (uint32_t)TEMP_GetCurrentValue();
		if (tempValue > 60) tempValue = 60;

		UART_AsyncTransmitCharacter('\b');
		UART_AsyncTransmitCharacter('\b');

		if (tempValue < 10) {
		    UART_AsyncTransmitCharacter(' ');
		} else {
		    UART_AsyncTransmitCharacter(tempValue / 10 + '0');
		}
		UART_AsyncTransmitCharacter(tempValue % 10 + '0');

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void ledCounter(TimerHandle_t xTimer) {
	UNUSED(xTimer);
	if(TEMP_GetCurrentValue() < temp_granica){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	} else {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
	}
}

void homeworkInit()
{
	LCD_Init();
	UART_Init();
	TEMP_Init();
	xTaskCreate(homeworkTask, "homeworkTask", 128, NULL, 5, NULL);
	ledTimer = xTimerCreate ("ledTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, ledCounter);
	xTimerStart(ledTimer, portMAX_DELAY);
}

