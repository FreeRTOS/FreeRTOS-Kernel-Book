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

/* Demo includes. */
#include "supporting_functions.h"

/* The task function. */
void vTaskFunction( void * pvParameters );

/* A variable that is incremented by the idle task hook function. */
static uint32_t ulIdleCycleCount = 0UL;

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char * pcTextForTask1 = "Task 1 is running\r\n";
const char * pcTextForTask2 = "Task 2 is running\r\n";

/*-----------------------------------------------------------*/

int main( void )
{
    /* Create the first task at priority 1... */
    xTaskCreate( vTaskFunction, "Task 1", 1000, ( void * ) pcTextForTask1, 1, NULL );

    /* ... and the second task at priority 2.  The priority is the second to
     * last parameter. */
    xTaskCreate( vTaskFunction, "Task 2", 1000, ( void * ) pcTextForTask2, 2, NULL );

    /* Start the scheduler to start the tasks executing. */
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

void vTaskFunction( void * pvParameters )
{
    char * pcTaskName;
    const TickType_t xDelay250ms = pdMS_TO_TICKS( 250UL );

    /* The string to print out is passed in via the parameter.  Cast this to a
     * character pointer. */
    pcTaskName = ( char * ) pvParameters;

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ; ; )
    {
        /* Print out the name of this task AND the number of times ulIdleCycleCount
         * has been incremented. */
        vPrintStringAndNumber( pcTaskName, ulIdleCycleCount );

        /* Delay for a period.  This time we use a call to vTaskDelay() which
         * puts the task into the Blocked state until the delay period has expired.
         * The delay period is specified in 'ticks'. */
        vTaskDelay( xDelay250ms );
    }
}
/*-----------------------------------------------------------*/

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
 * and return void. */
void vApplicationIdleHook( void )
{
    /* This hook function does nothing but increment a counter. */
    ulIdleCycleCount++;
}
