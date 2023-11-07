/*
 *  Copyright Amazon.com Inc. or its affiliates. All Rights Reserved.
 *
 *  SPDX-License-Identifier: MIT-0
 * 
 *  VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.
 *
 *  This file is part of the FreeRTOS distribution.
 * 
 *  This contains the Windows port implementation of the examples listed in the 
 *  FreeRTOS book Mastering_the_FreeRTOS_Real_Time_Kernel.
 *
 */

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "timers.h" /* For the xTimerPendFunctionCallFromISR() function. */

/* Demo includes. */
#include "supporting_functions.h"

/* The number of the simulated interrupt used in this example.  Numbers 0 to 2
 * are used by the FreeRTOS Windows port itself, so 3 is the first number available
 * to the application. */
#define mainINTERRUPT_NUMBER    3

/* Definitions for the event bits in the event group. */
#define mainFIRST_TASK_BIT      ( 1UL << 0UL ) /* Event bit 0, which is set by a task. */
#define mainSECOND_TASK_BIT     ( 1UL << 1UL ) /* Event bit 1, which is set by a task. */
#define mainISR_BIT             ( 1UL << 2UL ) /* Event bit 2, which is set by an ISR. */

/* The tasks to be created. */
static void vIntegerGenerator( void * pvParameters );
static void vEventBitSettingTask( void * pvParameters );
static void vEventBitReadingTask( void * pvParameters );

/* A function that can be deferred to run in the RTOS daemon task.  The function
 * prints out the string passed to it using the pvParameter1 parameter. */
void vPrintStringFromDaemonTask( void * pvParameter1,
                                 uint32_t ulParameter2 );

/* The service routine for the (simulated) interrupt.  This is the interrupt
 * that sets an event bit in the event group. */
static uint32_t ulEventBitSettingISR( void );

/*-----------------------------------------------------------*/

/* Declare the event group in which bits are set from both a task and an ISR. */
EventGroupHandle_t xEventGroup;

int main( void )
{
    /* Before an event group can be used it must first be created. */
    xEventGroup = xEventGroupCreate();

    /* Create the task that sets event bits in the event group. */
    xTaskCreate( vEventBitSettingTask, "BitSetter", 1000, NULL, 1, NULL );

    /* Create the task that waits for event bits to get set in the event
     * group. */
    xTaskCreate( vEventBitReadingTask, "BitReader", 1000, NULL, 2, NULL );

    /* Create the task that is used to periodically generate a software
     * interrupt. */
    xTaskCreate( vIntegerGenerator, "IntGen", 1000, NULL, 3, NULL );

    /* Install the handler for the software interrupt.  The syntax necessary
     * to do this is dependent on the FreeRTOS port being used.  The syntax
     * shown here can only be used with the FreeRTOS Windows port, where such
     * interrupts are only simulated. */
    vPortSetInterruptHandler( mainINTERRUPT_NUMBER, ulEventBitSettingISR );

    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();

    /* The following line should never be reached because vTaskStartScheduler()
    *  will only return if there was not enough FreeRTOS heap memory available to
    *  create the Idle and (if configured) Timer tasks.  Heap management, and
    *  techniques for trapping heap exhaustion, are described in the book text. */
    for( ; ; )
    {
    }

    return 0;
}
/*-----------------------------------------------------------*/

static void vEventBitSettingTask( void * pvParameters )
{
    const TickType_t xDelay200ms = pdMS_TO_TICKS( 200UL ), xDontBlock = 0;

    for( ; ; )
    {
        /* Delay for a short while before starting the next loop. */
        vTaskDelay( xDelay200ms );

        /* Print out a message to say event bit 0 is about to be set by the
         * task, then set event bit 0. */
        vPrintString( "Bit setting task -\t about to set bit 0.\r\n" );
        xEventGroupSetBits( xEventGroup, mainFIRST_TASK_BIT );

        /* Delay for a short while before setting the other bit set within this
         * task. */
        vTaskDelay( xDelay200ms );

        /* Print out a message to say event bit 1 is about to be set by the
         * task, then set event bit 1. */
        vPrintString( "Bit setting task -\t about to set bit 1.\r\n" );
        xEventGroupSetBits( xEventGroup, mainSECOND_TASK_BIT );
    }
}
/*-----------------------------------------------------------*/

static uint32_t ulEventBitSettingISR( void )
{
    BaseType_t xHigherPriorityTaskWoken;

/* The string is not printed within the interrupt service, but is instead
 * sent to the RTOS daemon task for printing.  It is therefore declared static to
 * ensure the compiler does not allocate the string on the stack of the ISR (as the
 * ISR's stack frame will not exist when the string is printed from the daemon
 * task. */
    static const char * pcString = "Bit setting ISR -\t about to set bit 2.\r\n";

    /* As always, xHigherPriorityTaskWoken is initialized to pdFALSE. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Print out a message to say bit 2 is about to be set.  Messages cannot be
     * printed from an ISR, so defer the actual output to the RTOS daemon task by
     * pending a function call to run in the context of the RTOS daemon task. */
    xTimerPendFunctionCallFromISR( vPrintStringFromDaemonTask, ( void * ) pcString, 0, &xHigherPriorityTaskWoken );

    /* Set bit 2 in the event group. */
    xEventGroupSetBitsFromISR( xEventGroup, mainISR_BIT, &xHigherPriorityTaskWoken );

    /* xEventGroupSetBitsFromISR() writes to the timer command queue.  If
     * writing to the timer command queue results in the RTOS daemon task leaving
     * the Blocked state, and if the priority of the RTOS daemon task is higher
     * than the priority of the currently executing task (the task this interrupt
     * interrupted) then xHigherPriorityTaskWoken will have been set to pdTRUE
     * inside xEventGroupSetBitsFromISR().
     *
     * xHigherPriorityTaskWoken is used as the parameter to portYIELD_FROM_ISR().
     * If xHigherPriorityTaskWoken equals pdTRUE then calling portYIELD_FROM_ISR()
     * will request a context switch.  If xHigherPriorityTaskWoken is still pdFALSE
     * then calling portYIELD_FROM_ISR() will have no effect.
     *
     * The implementation of portYIELD_FROM_ISR() used by the Windows port includes
     * a return statement, which is why this function does not explicitly return a
     * value. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static void vEventBitReadingTask( void * pvParameters )
{
    const EventBits_t xBitsToWaitFor = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainISR_BIT );
    EventBits_t xEventGroupValue;

    for( ; ; )
    {
        /* Block to wait for event bits to become set within the event group. */
        xEventGroupValue = xEventGroupWaitBits( /* The event group to read. */
            xEventGroup,

            /* Bits to test. */
            xBitsToWaitFor,

            /* Clear bits on exit if the
            *  unblock condition is met. */
            pdTRUE,

            /* Don't wait for all bits. */
            pdFALSE,

            /* Don't time out. */
            portMAX_DELAY );

        /* Print a message for each bit that was set. */
        if( ( xEventGroupValue & mainFIRST_TASK_BIT ) != 0 )
        {
            vPrintString( "Bit reading task -\t event bit 0 was set\r\n" );
        }

        if( ( xEventGroupValue & mainSECOND_TASK_BIT ) != 0 )
        {
            vPrintString( "Bit reading task -\t event bit 1 was set\r\n" );
        }

        if( ( xEventGroupValue & mainISR_BIT ) != 0 )
        {
            vPrintString( "Bit reading task -\t event bit 2 was set\r\n" );
        }

        vPrintString( "\r\n" );
    }
}
/*-----------------------------------------------------------*/

void vPrintStringFromDaemonTask( void * pvParameter1,
                                 uint32_t ulParameter2 )
{
    /* The string to print is passed into this function using the pvParameter1
     * parameter. */
    vPrintString( ( const char * ) pvParameter1 );
}
/*-----------------------------------------------------------*/

static void vIntegerGenerator( void * pvParameters )
{
    TickType_t xLastExecutionTime;
    const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

    /* Initialize the variable used by the call to vTaskDelayUntil(). */
    xLastExecutionTime = xTaskGetTickCount();

    for( ; ; )
    {
        /* This is a periodic task.  Block until it is time to run again.
         * The task will execute every 500ms. */
        vTaskDelayUntil( &xLastExecutionTime, xDelay500ms );

        /* Generate the interrupt that will set a bit in the event group. */
        vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER );
    }
}
