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
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
}

// GENERAL
// -----------------------------------------------------------------------------

void UART_Init()
{
}

// TRANSMIT UTIL
// -----------------------------------------------------------------------------

void UART_AsyncTransmitCharacter(char character)
{
}

void UART_AsyncTransmitString(char const *string)
{
}

void UART_AsyncTransmitDecimal(uint32_t decimal)
{
}
