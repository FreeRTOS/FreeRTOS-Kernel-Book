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
    /* Create the first task at priority 2.  This time the task parameter is
     * not used and is set to NULL.  The task handle is also not used so likewise
     * is also set to NULL. */
    xTaskCreate( vTask1, "Task 1", 1000, NULL, 2, NULL );
    /* The task is created at priority 2 ^. */

    /* Create the second task at priority 1 - which is lower than the priority
     * given to Task1.  Again the task parameter is not used so is set to NULL -
     * BUT this time we want to obtain a handle to the task so pass in the address
     * of the xTask2Handle variable. */
    xTaskCreate( vTask2, "Task 2", 1000, NULL, 1, &xTask2Handle );
    /* The task handle is the last parameter ^^^^^^^^^^^^^ */

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
    UBaseType_t uxPriority;

    /* This task will always run before Task2 as it has the higher priority.
     * Neither Task1 nor Task2 ever block so both will always be in either the
     * Running or the Ready state.
     *
     * Query the priority at which this task is running - passing in NULL means
     * "return our own priority". */
    uxPriority = uxTaskPriorityGet( NULL );

    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintString( "Task1 is running\r\n" );

        /* Setting the Task2 priority above the Task1 priority will cause
         * Task2 to immediately start running (as then Task2 will have the higher
         * priority of the    two created tasks). */
        vPrintString( "About to raise the Task2 priority\r\n" );
        vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );

        /* Task1 will only run when it has a priority higher than Task2.
         * Therefore, for this task to reach this point Task2 must already have
         * executed and set its priority back down to 0. */
    }
}

/*-----------------------------------------------------------*/

void vTask2( void * pvParameters )
{
    UBaseType_t uxPriority;

    /* Task1 will always run before this task as Task1 has the higher priority.
     * Neither Task1 nor Task2 ever block so will always be in either the
     * Running or the Ready state.
     *
     * Query the priority at which this task is running - passing in NULL means
     * "return our own priority". */
    uxPriority = uxTaskPriorityGet( NULL );

    for( ; ; )
    {
        /* For this task to reach this point Task1 must have already run and
         * set the priority of this task higher than its own.
         *
         * Print out the name of this task. */
        vPrintString( "Task2 is running\r\n" );

        /* Set our priority back down to its original value.  Passing in NULL
         * as the task handle means "change our own priority".  Setting the
         * priority below that of Task1 will cause Task1 to immediately start
         * running again. */
        vPrintString( "About to lower the Task2 priority\r\n" );
        vTaskPrioritySet( NULL, ( uxPriority - 2 ) );
    }
}
/*-----------------------------------------------------------*/
