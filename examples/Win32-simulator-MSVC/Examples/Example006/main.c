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

/* The task functions. */
void vContinuousProcessingTask( void * pvParameters );
void vPeriodicTask( void * pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
 * defined const and off the stack to ensure they remain valid when the tasks are
 * executing. */
const char * pcTextForTask1 = "Continuous task 1 running\r\n";
const char * pcTextForTask2 = "Continuous task 2 running\r\n";
const char * pcTextForPeriodicTask = "Periodic task is running\r\n";

/*-----------------------------------------------------------*/

int main( void )
{
    /* Create two instances of the continuous processing task, both at priority	1. */
    xTaskCreate( vContinuousProcessingTask, "Task 1", 1000, ( void * ) pcTextForTask1, 1, NULL );
    xTaskCreate( vContinuousProcessingTask, "Task 2", 1000, ( void * ) pcTextForTask2, 1, NULL );

    /* Create one instance of the periodic task at priority 2. */
    xTaskCreate( vPeriodicTask, "Task 3", 1000, ( void * ) pcTextForPeriodicTask, 2, NULL );

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

void vContinuousProcessingTask( void * pvParameters )
{
    char * pcTaskName;

    /* The string to print out is passed in via the parameter.  Cast this to a
     * character pointer. */
    pcTaskName = ( char * ) pvParameters;

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ; ; )
    {
        /* Print out the name of this task.  This task just does this repeatedly
         * without ever blocking or delaying. */
        vPrintString( pcTaskName );
    }
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void * pvParameters )
{
    TickType_t xLastWakeTime;
    const TickType_t xDelay5ms = pdMS_TO_TICKS( 3UL );

    /* The xLastWakeTime variable needs to be initialized with the current tick
     * count.  Note that this is the only time we access this variable.  From this
     * point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
     * API function. */
    xLastWakeTime = xTaskGetTickCount();

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintString( "Periodic task is running\r\n" );

        /* We want this task to execute exactly every 10 milliseconds. */
        vTaskDelayUntil( &xLastWakeTime, xDelay5ms );
    }
}
