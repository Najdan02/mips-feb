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

static void homeworkTask(void *parameters)
{
	
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
	TEMP_Init();
	ledTimer = xTimerCreate ("ledTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, ledCounter);
	xTimerStart(ledTimer, portMAX_DELAY);
}

