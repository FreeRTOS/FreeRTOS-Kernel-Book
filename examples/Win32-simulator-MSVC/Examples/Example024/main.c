/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Demo includes. */
#include "supporting_functions.h"

/* The number of the simulated interrupt used in this example.  Numbers 0 to 2
are used by the FreeRTOS Windows port itself, so 3 is the first number available
to the application. */
#define mainINTERRUPT_NUMBER	3

/* The tasks to be created. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

/* The service routine for the (simulated) interrupt.  This is the interrupt
that the task will be synchronized with. */
static uint32_t ulExampleInterruptHandler( void );

/* The rate at which the periodic task generates software interrupts. */
static const TickType_t xInterruptFrequency = pdMS_TO_TICKS( 500UL );

/* Stores the handle of the task to which interrupt processing is deferred. */
static TaskHandle_t xHandlerTask = NULL;

/*-----------------------------------------------------------*/

int main( void )
{
	/* Create the 'handler' task, which is the task to which interrupt
	processing is deferred, and so is the task that will be synchronized
	with the interrupt.  The handler task is created with a high priority to
	ensure it runs immediately after the interrupt exits.  In this case a
	priority of 3 is chosen.  The handle of the task is saved for use by the
	ISR. */
	xTaskCreate( vHandlerTask, "Handler", 1000, NULL, 3, &xHandlerTask );

	/* Create the task that will periodically generate a software interrupt.
	This is created with a priority below the handler task to ensure it will
	get preempted each time the handler task exits the Blocked state. */
	xTaskCreate( vPeriodicTask, "Periodic", 1000, NULL, 1, NULL );

	/* Install the handler for the software interrupt.  The syntax necessary
	to do this is dependent on the FreeRTOS port being used.  The syntax
	shown here can only be used with the FreeRTOS Windows port, where such
	interrupts are only simulated. */
	vPortSetInterruptHandler( mainINTERRUPT_NUMBER, ulExampleInterruptHandler );

	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

static void vHandlerTask( void *pvParameters )
{
/* xMaxExpectedBlockTime is set to be a little longer than the maximum expected
time between events. */
const TickType_t xMaxExpectedBlockTime = xInterruptFrequency + pdMS_TO_TICKS( 10 );
uint32_t ulEventsToProcess;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Wait to receive a notification sent directly to this task from the
		interrupt handler. */
		ulEventsToProcess = ulTaskNotifyTake( pdTRUE, xMaxExpectedBlockTime );
		if( ulEventsToProcess != 0 )
		{
			/* To get here at least one event must have occurred.  Loop here
			until all the pending events have been processed (in this case, just
			print out a message for each event). */
			while( ulEventsToProcess > 0 )
			{
				vPrintString( "Handler task - Processing event.\r\n" );
				ulEventsToProcess--;
			}
		}
		else
		{
			/* If this part of the function is reached then an interrupt did not
			arrive within the expected time, and (in a real application) it may
			be necessary to perform some error recovery operations. */
		}
	}
}
/*-----------------------------------------------------------*/

static uint32_t ulExampleInterruptHandler( void )
{
BaseType_t xHigherPriorityTaskWoken;

	/* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
	it will get set to pdTRUE inside the interrupt safe API function if a
	context switch is required. */
	xHigherPriorityTaskWoken = pdFALSE;

	/* Send a notification directly to the handler task. */
	vTaskNotifyGiveFromISR( /* The handle of the task to which the notification
							is being sent.  The handle was saved when the task
							was created. */
							xHandlerTask,

							/* xHigherPriorityTaskWoken is used in the usual
							way. */
							&xHigherPriorityTaskWoken );

	/* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().  If
	xHigherPriorityTaskWoken was set to pdTRUE inside vTaskNotifyGiveFromISR()
	then calling portYIELD_FROM_ISR() will request a context switch.  If
	xHigherPriorityTaskWoken is still pdFALSE then calling
	portYIELD_FROM_ISR() will have no effect.  The implementation of
	portYIELD_FROM_ISR() used by the Windows port includes a return statement,
	which is why this function does not explicitly return a value. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void *pvParameters )
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* This task is just used to 'simulate' an interrupt.  This is done by
		periodically generating a simulated software interrupt.  Block until it
		is time to generate the software interrupt again. */
		vTaskDelay( xInterruptFrequency );

		/* Generate the interrupt, printing a message both before and after
		the interrupt has been generated so the sequence of execution is evident
		from the output.

		The syntax used to generate a software interrupt is dependent on the
		FreeRTOS port being used.  The syntax used below can only be used with
		the FreeRTOS Windows port, in which such interrupts are only
		simulated. */
		vPrintString( "Periodic task - About to generate an interrupt.\r\n" );
		vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER );
		vPrintString( "Periodic task - Interrupt generated.\r\n\r\n\r\n" );
	}
}









