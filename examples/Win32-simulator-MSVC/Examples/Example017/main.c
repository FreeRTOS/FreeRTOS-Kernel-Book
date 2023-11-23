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
#include "semphr.h"

/* Demo includes. */
#include "supporting_functions.h"


/* The number of the simulated interrupt used in this example.  Numbers 0 to 2
 * are used by the FreeRTOS Windows port itself, so 3 is the first number available
 * to the application. */
#define mainINTERRUPT_NUMBER    3

/* The tasks to be created. */
static void vHandlerTask( void * pvParameters );
static void vPeriodicTask( void * pvParameters );

/* The service routine for the (simulated) interrupt.  This is the interrupt
 * that the task will be synchronized with. */
static uint32_t ulExampleInterruptHandler( void );

/*-----------------------------------------------------------*/

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
 * semaphore that is used to synchronize a task with an interrupt. */
SemaphoreHandle_t xCountingSemaphore;


int main( void )
{
    /* Before a semaphore is used it must be explicitly created.  In this
     * example a counting semaphore is created.  The semaphore is created to have a
     * maximum count value of 10, and an initial count value of 0. */
    xCountingSemaphore = xSemaphoreCreateCounting( 10, 0 );

    /* Check the semaphore was created successfully. */
    if( xCountingSemaphore != NULL )
    {
        /* Create the 'handler' task, which is the task to which interrupt
         * processing is deferred, and so is the task that will be synchronized
         * with the interrupt.  The handler task is created with a high priority to
         * ensure it runs immediately after the interrupt exits.  In this case a
         * priority of 3 is chosen. */
        xTaskCreate( vHandlerTask, "Handler", 1000, NULL, 3, NULL );

        /* Create the task that will periodically generate a software interrupt.
         * This is created with a priority below the handler task to ensure it will
         * get preempted each time the handler task exits the Blocked state. */
        xTaskCreate( vPeriodicTask, "Periodic", 1000, NULL, 1, NULL );

        /* Install the handler for the software interrupt.  The syntax necessary
         * to do this is dependent on the FreeRTOS port being used.  The syntax
         * shown here can only be used with the FreeRTOS Windows port, where such
         * interrupts are only simulated. */
        vPortSetInterruptHandler( mainINTERRUPT_NUMBER, ulExampleInterruptHandler );

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

static void vHandlerTask( void * pvParameters )
{
    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* Use the semaphore to wait for the event.  The semaphore was created
         * before the scheduler was started so before this task ran for the first
         * time.  The task blocks indefinitely meaning this function call will only
         * return once the semaphore has been successfully obtained - so there is
         * no need to check the returned value. */
        xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );

        /* To get here the event must have occurred.  Process the event (in this
         * case just print out a message). */
        vPrintString( "Handler task - Processing event.\r\n" );
    }
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void * pvParameters )
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );

    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* This task is just used to 'simulate' an interrupt.  This is done by
         * periodically generating a simulated software interrupt.  Block until it
         * is time to generate the software interrupt again. */
        vTaskDelay( xDelay500ms );

        /* Generate the interrupt, printing a message both before and after
         * the interrupt has been generated so the sequence of execution is evident
         * from the output.
         *
         * The syntax used to generate a software interrupt is dependent on the
         * FreeRTOS port being used.  The syntax used below can only be used with
         * the FreeRTOS Windows port, in which such interrupts are only
         * simulated. */
        vPrintString( "Periodic task - About to generate an interrupt.\r\n" );
        vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER );
        vPrintString( "Periodic task - Interrupt generated.\r\n\r\n\r\n" );
    }
}
/*-----------------------------------------------------------*/

static uint32_t ulExampleInterruptHandler( void )
{
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     * it will get set to pdTRUE inside the interrupt safe API function if a
     * context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' the semaphore multiple times.  The first will unblock the deferred
     * interrupt handling task, the following 'gives' are to demonstrate that the
     * semaphore latches the events to allow the handler task to process them in
     * turn without events getting lost.  This simulates multiple interrupts being
     * processed by the processor, even though in this case the events are
     * simulated within a single interrupt occurrence.*/
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );
    xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().  If
     * xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
     * then calling portYIELD_FROM_ISR() will request a context switch.  If
     * xHigherPriorityTaskWoken is still pdFALSE then calling
     * portYIELD_FROM_ISR() will have no effect.  The implementation of
     * portYIELD_FROM_ISR() used by the Windows port includes a return statement,
     * which is why this function does not explicitly return a value. */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
