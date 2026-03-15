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

static unsigned rainfall = 0;
static unsigned passedMs = 1;
static TimerHandle_t homeworkTimer;
static char tempText[4];


static void homeworkTask(void *parameters)
{
	UNUSED(parameters);
	char messageTemp[9] = "Temp:   ";
	char messageKisa[7] = "Kisa: ";

	for (uint32_t i = 0; i < 6; i++) {
	    LCD_CommandEnqueue(LCD_DATA, messageKisa[i]);
	}

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

		itoa(rainfall, tempText, 10);

		LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x06);

		for (uint32_t i = 0; i < strlen(tempText); i++) {
		    LCD_CommandEnqueue(LCD_DATA, tempText[i]);
		}

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}

void homeworkOverflow(void) {
	if (passedMs > 0) {
		/* Ceiling division: round up to avoid "1 less" from truncation */
		rainfall = (36000 + passedMs - 1) / passedMs;
	}
	passedMs = 0;
}

void homeworkCounter(TimerHandle_t xTimer) {
    UNUSED(xTimer);
    ++passedMs;
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
	homeworkTimer = xTimerCreate("homeworkTimer", pdMS_TO_TICKS(1), pdTRUE, NULL, homeworkCounter);
	xTimerStart(homeworkTimer, portMAX_DELAY);
	ledTimer = xTimerCreate ("ledTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, ledCounter);
	xTimerStart(ledTimer, portMAX_DELAY);
}

