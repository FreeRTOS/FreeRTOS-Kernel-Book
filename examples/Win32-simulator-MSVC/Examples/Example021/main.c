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

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo includes. */
#include "supporting_functions.h"

/* The task that sends messages to the stdio gatekeeper.  Two instances of this
 * task are created. */
static void prvPrintTask( void * pvParameters );

/* The gatekeeper task itself. */
static void prvStdioGatekeeperTask( void * pvParameters );

/* Define the strings that the tasks and interrupt will print out via the
 * gatekeeper. */
static const char * pcStringsToPrint[] =
{
    "Task 1 ****************************************************\r\n",
    "Task 2 ----------------------------------------------------\r\n",
    "Message printed from the tick hook interrupt ##############\r\n"
};

/*-----------------------------------------------------------*/

/* Declare a variable of type QueueHandle_t.  This is used to send messages from
 * the print tasks to the gatekeeper task. */
static QueueHandle_t xPrintQueue;

/* The tasks block for a pseudo random time between 0 and xMaxBlockTime ticks. */
const TickType_t xMaxBlockTimeTicks = 0x20;

int main( void )
{
    /* Before a queue is used it must be explicitly created.  The queue is created
     * to hold a maximum of 5 character pointers. */
    xPrintQueue = xQueueCreate( 5, sizeof( char * ) );

    /* Check the queue was created successfully. */
    if( xPrintQueue != NULL )
    {
        /* Create two instances of the tasks that send messages to the gatekeeper.
         * The	index to the string they attempt to write is passed in as the task
         * parameter (4th parameter to xTaskCreate()).  The tasks are created at
         * different priorities so some pre-emption will occur. */
        xTaskCreate( prvPrintTask, "Print1", 1000, ( void * ) 0, 1, NULL );
        xTaskCreate( prvPrintTask, "Print2", 1000, ( void * ) 1, 2, NULL );

        /* Create the gatekeeper task.  This is the only task that is permitted
         * to access standard out. */
        xTaskCreate( prvStdioGatekeeperTask, "Gatekeeper", 1000, NULL, 0, NULL );

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }

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

static void prvStdioGatekeeperTask( void * pvParameters )
{
    char * pcMessageToPrint;

    /* This is the only task that is allowed to write to the terminal output.
     * Any other task wanting to write to the output does not access the terminal
     * directly, but instead sends the output to this task.  As only one task
     * writes to standard out there are no mutual exclusion or serialization issues
     * to consider within this task itself. */
    for( ; ; )
    {
        /* Wait for a message to arrive. */
        xQueueReceive( xPrintQueue, &pcMessageToPrint, portMAX_DELAY );

        /* There is no need to check the return	value as the task will block
         * indefinitely and only run again when a message has arrived.  When the
         * next line is executed there will be a message to be output. */
        printf( "%s", pcMessageToPrint );
        fflush( stdout );

        /* Now simply go back to wait for the next message. */
    }
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    static int iCount = 0;

    /* Print out a message every 200 ticks.  The message is not written out
     * directly, but sent to the gatekeeper task. */
    iCount++;

    if( iCount >= 200 )
    {
        /* In this case the last parameter (xHigherPriorityTaskWoken) is not
         * actually used and is set to NULL. */
        xQueueSendToFrontFromISR( xPrintQueue, &( pcStringsToPrint[ 2 ] ), NULL );

        /* Reset the count ready to print out the string again in 200 ticks
         * time. */
        iCount = 0;
    }
}
/*-----------------------------------------------------------*/

static void prvPrintTask( void * pvParameters )
{
    int iIndexToString;

    /* Two instances of this task are created so the index to the string the task
     * will send to the gatekeeper task is passed in the task parameter.  Cast this
     * to the required type. */
    iIndexToString = ( int ) pvParameters;

    for( ; ; )
    {
        /* Print out the string, not directly but by passing the string to the
         * gatekeeper task on the queue.  The queue is created before the scheduler is
         * started so will already exist by the time this task executes.  A block time
         * is not specified as there should always be space in the queue. */
        xQueueSendToBack( xPrintQueue, &( pcStringsToPrint[ iIndexToString ] ), 0 );

        /* Wait a pseudo random time.  Note that rand() is not necessarily
         * re-entrant, but in this case it does not really matter as the code does
         * not care what value is returned.  In a more secure application a version
         * of rand() that is known to be re-entrant should be used - or calls to
         * rand() should be protected using a critical section. */
        vTaskDelay( ( rand() % xMaxBlockTimeTicks ) );
    }
}
/*-----------------------------------------------------------*/



/* In other examples this function is implemented within the
 * supporting_functions.c source file - but that source file is not included in
 * this example as to include it would result in multiple definitions of the tick
 * hook function. */
void vAssertCalled( uint32_t ulLine,
                    const char * const pcFile )
{
/* The following two variables are just to ensure the parameters are not
*  optimised away and therefore unavailable when viewed in the debugger. */
    volatile uint32_t ulLineNumber = ulLine, ulSetNonZeroInDebuggerToReturn = 0;
    volatile const char * const pcFileName = pcFile;

    taskENTER_CRITICAL();
    {
        while( ulSetNonZeroInDebuggerToReturn == 0 )
        {
            /* If you want to set out of this function in the debugger to see
             * the	assert() location then set ulSetNonZeroInDebuggerToReturn to a
             * non-zero value. */
        }
    }
    taskEXIT_CRITICAL();

    /* Remove the potential for compiler warnings issued because the variables
     * are set but not subsequently referenced. */
    ( void ) pcFileName;
    ( void ) ulLineNumber;
}
/*-----------------------------------------------------------*/

/* In other examples this function is implemented within the
 * supporting_functions.c source file - but that source file is not included in
 * this example as to include it would result in multiple definitions of the tick
 * hook function. */
void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer, event group, or semaphore is created.  It is also called by various
     * parts of the demo application.  If heap_1.c, heap_2.c or heap_4.c are used,
     * then the size of the heap available to pvPortMalloc() is defined by
     * configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
     * API function can be used to query the size of free heap space that remains.
     * More information is provided in the book text. */
    vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/
