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

/* The two task functions. */
void vTask1( void * pvParameters );
void vTask2( void * pvParameters );

/* Used to hold the handle of Task2. */
TaskHandle_t xTask2Handle;

/*-----------------------------------------------------------*/

int main( void )
{
    /* Create the first task at priority 1.  This time the task parameter is
     * not used and is set to NULL.  The task handle is also not used so likewise
     * is also set to NULL. */
    xTaskCreate( vTask1, "Task 1", 1000, NULL, 1, NULL );
    /* The task is created at priority 1 ^. */

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

void vTask1( void * pvParameters )
{
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );

    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintString( "Task1 is running\r\n" );

        /* Create task 2 at a higher priority.  Again the task parameter is not
         * used so is set to NULL - BUT this time we want to obtain a handle to the
         * task so pass in the address of the xTask2Handle variable. */
        xTaskCreate( vTask2, "Task 2", 1000, NULL, 2, &xTask2Handle );
        /* The task handle is the last parameter ^^^^^^^^^^^^^ */

        /* Task2 has/had the higher priority, so for Task1 to reach here Task2
         * must have already executed and deleted itself.  Delay for 100
         * milliseconds. */
        vTaskDelay( xDelay100ms );
    }
}

/*-----------------------------------------------------------*/

void vTask2( void * pvParameters )
{
    /* Task2 does nothing but delete itself.  To do this it could call vTaskDelete()
     * using a NULL parameter, but instead and purely for demonstration purposes it
     * instead calls vTaskDelete() with its own task handle. */
    vPrintString( "Task2 is running and about to delete itself\r\n" );
    vTaskDelete( xTask2Handle );
}
/*-----------------------------------------------------------*/
