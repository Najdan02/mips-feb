/*
 * driver_uart.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "driver_uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>

#include "usart.h"

// TRANSMIT
// -----------------------------------------------------------------------------

static TaskHandle_t UART_TransmitTaskHandle;
static QueueHandle_t UART_TransmitQueueHandle;
static SemaphoreHandle_t UART_TransmitMutexHandle;

static void UART_TransmitTask(void *parameters)
{
	while (1) {
		char c;
		xQueueReceive(UART_TransmitQueueHandle, &c, portMAX_DELAY);
		HAL_UART_Transmit(&huart1, &c, sizeof(char), HAL_MAX_DELAY);
		xSemaphoreGive(UART_TransmitMutexHandle);
		xSemaphoreTake(UART_TransmitMutexHandle, portMAX_DELAY);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t woken = pdFALSE;
	xSemaphoreGiveFromISR(UART_TransmitMutexHandle, &woken);
	portYIELD_FROM_ISR(woken);
}

// GENERAL
// -----------------------------------------------------------------------------

void UART_Init()
{
	UART_TransmitQueueHandle = xQueueCreate(64, sizeof(char));
	UART_TransmitMutexHandle = xSemaphoreCreateBinary();
	xTaskCreate(UART_TransmitTask, "UARTtx", 64, NULL, 3, &UART_TransmitTaskHandle);
}

// TRANSMIT UTIL
// -----------------------------------------------------------------------------

void UART_AsyncTransmitCharacter(char character)
{
	xQueueSend(UART_TransmitQueueHandle, &character, portMAX_DELAY);
}

void UART_AsyncTransmitString(char const *string)
{
	for (int i = 0; i < strlen(string); i++) UART_AsyncTransmitCharacter(string[i]);
}

void UART_AsyncTransmitDecimal(uint32_t decimal)
{
	char str[10];
	itoa(decimal, &str, 10);
	UART_AsyncTransmitString(str);
}
