/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "supporting_functions.h"

/* The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type QueueHandle_t.  This is used to store the queue
that is accessed by all three tasks. */
QueueHandle_t xQueue;

typedef enum
{
	eSender1,
	eSender2
} DataSource_t;

/* Define the structure type that will be passed on the queue. */
typedef struct
{
	uint8_t ucValue;
	DataSource_t eDataSource;
} Data_t;

/* Declare two variables of type Data_t that will be passed on the queue. */
static const Data_t xStructsToSend[ 2 ] =
{
	{ 100, eSender1 }, /* Used by Sender1. */
	{ 200, eSender2 }  /* Used by Sender2. */
};

int main( void )
{
    /* The queue is created to hold a maximum of 3 structures of type Data_t. */
    xQueue = xQueueCreate( 3, sizeof( Data_t ) );

	if( xQueue != NULL )
	{
		/* Create two instances of the task that will write to the queue.  The
		parameter is used to pass the structure that the task should write to the
		queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		while the other task will continuously send xStructsToSend[ 1 ].  Both
		tasks are created at priority 2, which is above the priority of the receiver. */
		xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
		xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );

		/* Create the task that will read from the queue.  The task is created with
		priority 1, so below the priority of the sender tasks. */
		xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 1, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	else
	{
		/* The queue could not be created. */
	}

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

static void vSenderTask( void *pvParameters )
{
BaseType_t xStatus;
const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* The first parameter is the queue to which data is being sent.  The
		queue was created before the scheduler was started, so before this task
		started to execute.

		The second parameter is the address of the structure being sent.  The
		address is passed in as the task parameter.

		The third parameter is the Block time - the time the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  A block time is specified as the queue
		will become full.  Items will only be removed from the queue when both
		sending tasks are in the Blocked state.. */
		xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait );

		if( xStatus != pdPASS )
		{
			/* We could not write to the queue because it was full - this must
			be an error as the receiving task should make space in the queue
			as soon as both sending tasks are in the Blocked state. */
			vPrintString( "Could not send to the queue.\r\n" );
		}
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
/* Declare the structure that will hold the values received from the queue. */
Data_t xReceivedStructure;
BaseType_t xStatus;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task only runs when the sending tasks are in the Blocked state,
		and the sending tasks only block when the queue is full, this task should
		always find the queue to be full.  3 is the queue length. */
		if( uxQueueMessagesWaiting( xQueue ) != 3 )
		{
			vPrintString( "Queue should have been full!\r\n" );
		}

		/* The first parameter is the queue from which data is to be received.  The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of a variable that
		has the required size to hold the received structure.

		The last parameter is the block time - the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available
		should the queue already be empty.  A block time is not necessary as this
		task will only run when the queue is full so data will always be available. */
		xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value and the source of the value. */
			if( xReceivedStructure.eDataSource == eSender1 )
			{
				vPrintStringAndNumber( "From Sender 1 = ", xReceivedStructure.ucValue );
			}
			else
			{
				vPrintStringAndNumber( "From Sender 2 = ", xReceivedStructure.ucValue );
			}
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error
			as this task should only run when the queue is full. */
			vPrintString( "Could not receive from the queue.\r\n" );
		}
	}
}






