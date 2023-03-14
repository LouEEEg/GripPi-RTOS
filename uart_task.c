/*
 * uart_task.c
 *
 *  Created on: Mar 13, 2023
 *      Author: louiskraft
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "switch_task.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
// UART Parameters
//
//*****************************************************************************
#define UART_BUFFERED 1

#define UARTTASKSTACKSIZE        200         // Stack size in words

extern xQueueHandle g_pLEDQueue;
extern xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// This task reads the buttons' state and passes this information to LEDTask.
//
//*****************************************************************************

static void
UartTask(void *pvParameters)
{
    portTickType ui16LastTime;
    uint32_t ui32SwitchDelay = 25;

    int uart_rdata_state = 0;
    char *uart_rdata = (char*)pvPortMalloc(32*sizeof(char));

    ui16LastTime = xTaskGetTickCount();

    while(1)
    {

        uart_rdata_state = UARTgets(uart_rdata, 32);
        UARTprintf("%c", uart_rdata);

        vTaskDelayUntil(&ui16LastTime, ui32SwitchDelay / portTICK_RATE_MS);

    }

}


uint32_t
UartTaskInit(void)
{
    if(xTaskCreate(UartTask, (const portCHAR *)"Uart",
                   UARTTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_UART_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return(0);
}
