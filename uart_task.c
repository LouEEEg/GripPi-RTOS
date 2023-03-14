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
#include "utils/ustdlib.h"
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
#define UART_BUFFERED 32

#define UARTTASKSTACKSIZE        500         // Stack size in words

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
    uint8_t ui8Message;
    unsigned long temp;

    int uart_rdata_state = 0;
    char *uart_rdata  = (char*)pvPortMalloc(32*sizeof(char));
    char **uart_rdata_end = (char**)pvPortMalloc(32*sizeof(char));

    ui16LastTime = xTaskGetTickCount();

    while(1)
    {

        uart_rdata_state = UARTgets(uart_rdata, 32);
        temp = ustrtoul(uart_rdata, uart_rdata_end, 10);
        ui8Message = (uint8_t)temp;
        UARTprintf("%i int: \n", ui8Message);
        UARTprintf("%c \n", uart_rdata);


        //
        // Pass the value of the button pressed to LEDTask.
        //
        if(xQueueSend(g_pLEDQueue, &ui8Message, portMAX_DELAY) !=
           pdPASS)
        {
            //
            // Error. The queue should never be full. If so print the
            // error message on UART and wait for ever.
            //
            UARTprintf("\nQueue full. This should never happen.\n");
            while(1)
            {
            }
        }

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
