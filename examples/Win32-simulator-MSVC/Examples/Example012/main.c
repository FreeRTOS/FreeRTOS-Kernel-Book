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

/* The three sender task. */
void vSenderTask1( void * pvParameters );
void vSenderTask2( void * pvParameters );

/* The receiver task.  The receiver blocks on the queue set to received data
 * from both sender task. */
void vReceiverTask( void * pvParameters );

/*-----------------------------------------------------------*/

/* Declare two variables of type QueueHandle_t.  Both queues are added to the
 * same queue set. */
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;

/* Declare a variable of type QueueSetHandle_t.  This is the queue set to which
 * the two queues are added. */
static QueueSetHandle_t xQueueSet = NULL;

int main( void )
{
    /* Create the two queues.  Each queue sends character pointers.  The
     * priority of the receiving task is above the priority of the sending tasks so
     * the queues will never have more than one item in them at any one time. */
    xQueue1 = xQueueCreate( 1, sizeof( char * ) );
    xQueue2 = xQueueCreate( 1, sizeof( char * ) );

    /* Create the queue set.  There are two queues both of which can contain
     * 1 item, so the maximum number of queue handle the queue set will ever have
     * to hold is 2 (1 item multiplied by 2 sets). */
    xQueueSet = xQueueCreateSet( 1 * 2 );

    /* Add the two queues to the set. */
    xQueueAddToSet( xQueue1, xQueueSet );
    xQueueAddToSet( xQueue2, xQueueSet );

    /* Create the tasks that send to the queues. */
    xTaskCreate( vSenderTask1, "Sender1", 1000, NULL, 1, NULL );
    xTaskCreate( vSenderTask2, "Sender2", 1000, NULL, 1, NULL );

    /* Create the receiver task. */
    xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );

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

static void vSenderTask1( void * pvParameters )
{
    const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
    const char * const pcMessage = "Message from vSenderTask1\r\n";

    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* Block for 100ms. */
        vTaskDelay( xBlockTime );

        /* Send this task's string to xQueue1. It is not necessary to use a
         * block time, even though the queue can only hold one item.  This is
         * because the priority of the task that reads from the queue is higher
         * than the priority of this task; as soon as this task writes to the queue
         * it will be pre-empted by the task that reads from the queue, so the
         * queue will already be empty again by the time the call to xQueueSend()
         * returns.  The block time is set to 0. */
        xQueueSend( xQueue1, &pcMessage, 0 );
    }
}
/*-----------------------------------------------------------*/

static void vSenderTask2( void * pvParameters )
{
    const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
    const char * const pcMessage = "Message from vSenderTask2\r\n";

    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* Block for 200ms. */
        vTaskDelay( xBlockTime );

        /* Send this task's string to xQueue1. It is not necessary to use a
         * block time, even though the queue can only hold one item.  This is
         * because the priority of the task that reads from the queue is higher
         * than the priority of this task; as soon as this task writes to the queue
         * it will be pre-empted by the task that reads from the queue, so the
         * queue will already be empty again by the time the call to xQueueSend()
         * returns.  The block time is set to 0. */
        xQueueSend( xQueue2, &pcMessage, 0 );
    }
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void * pvParameters )
{
    QueueHandle_t xQueueThatContainsData;
    char * pcReceivedString;

    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ; ; )
    {
        /* Block on the queue set to wait for one of the queues in the set to
         * contain data.  Cast the QueueSetMemberHandle_t values returned from
         * xQueueSelectFromSet() to a QueueHandle_t as it is known that all the
         * items in the set are queues (as opposed to semaphores, which can also be
         * members of a queue set). */
        xQueueThatContainsData = ( QueueHandle_t ) xQueueSelectFromSet( xQueueSet, portMAX_DELAY );

        /* An indefinite block time was used when reading from the set so
         * xQueueSelectFromSet() will not have returned unless one of the queues in
         * the set contained data, and xQueueThatContansData must be valid.  Read
         * from the queue.  It is not necessary to specify a block time because it
         * is known that the queue contains data.  The block time is set to 0. */
        xQueueReceive( xQueueThatContainsData, &pcReceivedString, 0 );

        /* Print the string received from the queue. */
        vPrintString( pcReceivedString );
    }
}
