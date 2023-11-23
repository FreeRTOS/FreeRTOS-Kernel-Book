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
#include "queue.h"

/* Demo includes. */
#include "supporting_functions.h"


/* The tasks to be created.  Two instances are created of the sender task while
 * only a single instance is created of the receiver task. */
static void vSenderTask( void * pvParameters );
static void vReceiverTask( void * pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type QueueHandle_t.  This is used to store the queue
 * that is accessed by all three tasks. */
QueueHandle_t xQueue;


int main( void )
{
    /* The queue is created to hold a maximum of 5 long values. */
    xQueue = xQueueCreate( 5, sizeof( int32_t ) );

    if( xQueue != NULL )
    {
        /* Create two instances of the task that will write to the queue.  The
         * parameter is used to pass the value that the task should write to the queue,
         * so one task will continuously write 100 to the queue while the other task
         * will continuously write 200 to the queue.  Both tasks are created at
         * priority 1. */
        xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) 100, 1, NULL );
        xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) 200, 1, NULL );

        /* Create the task that will read from the queue.  The task is created with
         * priority 2, so above the priority of the sender tasks. */
        xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }
    else
    {
        /* The queue could not be created. */
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

static void vSenderTask( void * pvParameters )
{
    int32_t lValueToSend;
    BaseType_t xStatus;

    /* Two instances are created of this task so the value that is sent to the
     * queue is passed in via the task parameter rather than be hard coded.  This way
     * each instance can use a different value.  Cast the parameter to the required
     * type. */
    lValueToSend = ( int32_t ) pvParameters;

    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* The first parameter is the queue to which data is being sent.  The
         * queue was created before the scheduler was started, so before this task
         * started to execute.
         *
         * The second parameter is the address of the data to be sent.
         *
         * The third parameter is the Block time � the time the task should be kept
         * in the Blocked state to wait for space to become available on the queue
         * should the queue already be full.  In this case we don�t specify a block
         * time because there should always be space in the queue. */
        xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

        if( xStatus != pdPASS )
        {
            /* We could not write to the queue because it was full � this must
             * be an error as the queue should never contain more than one item! */
            vPrintString( "Could not send to the queue.\r\n" );
        }
    }
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void * pvParameters )
{
/* Declare the variable that will hold the values received from the queue. */
    int32_t lReceivedValue;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );

    /* This task is also defined within an infinite loop. */
    for( ; ; )
    {
        /* As this task unblocks immediately that data is written to the queue this
         * call should always find the queue empty. */
        if( uxQueueMessagesWaiting( xQueue ) != 0 )
        {
            vPrintString( "Queue should have been empty!\r\n" );
        }

        /* The first parameter is the queue from which data is to be received.  The
         * queue is created before the scheduler is started, and therefore before this
         * task runs for the first time.
         *
         * The second parameter is the buffer into which the received data will be
         * placed.  In this case the buffer is simply the address of a variable that
         * has the required size to hold the received data.
         *
         * the last parameter is the block time � the maximum amount of time that the
         * task should remain in the Blocked state to wait for data to be available should
         * the queue already be empty. */
        xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

        if( xStatus == pdPASS )
        {
            /* Data was successfully received from the queue, print out the received
             * value. */
            vPrintStringAndNumber( "Received = ", lReceivedValue );
        }
        else
        {
            /* We did not receive anything from the queue even after waiting for 100ms.
             * This must be an error as the sending tasks are free running and will be
             * continuously writing to the queue. */
            vPrintString( "Could not receive from the queue.\r\n" );
        }
    }
}
