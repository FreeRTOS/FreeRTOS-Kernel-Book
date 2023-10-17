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

/* Demo includes. */
#include "supporting_functions.h"

/* The task functions. */
void vContinuousProcessingTask( void *pvParameters );
void vPeriodicTask( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Continuous task 1 running\r\n";
const char *pcTextForTask2 = "Continuous task 2 running\r\n";
const char *pcTextForPeriodicTask = "Periodic task is running\r\n";

/*-----------------------------------------------------------*/

int main( void )
{
	/* Create two instances of the continuous processing task, both at priority	1. */
	xTaskCreate( vContinuousProcessingTask, "Task 1", 1000, (void*)pcTextForTask1, 1, NULL );
	xTaskCreate( vContinuousProcessingTask, "Task 2", 1000, (void*)pcTextForTask2, 1, NULL );

	/* Create one instance of the periodic task at priority 2. */
	xTaskCreate( vPeriodicTask, "Task 3", 1000, (void*)pcTextForPeriodicTask, 2, NULL );

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();

	/* The following line should never be reached because vTaskStartScheduler()
	will only return if there was not enough FreeRTOS heap memory available to
	create the Idle and (if configured) Timer tasks.  Heap management, and
	techniques for trapping heap exhaustion, are described in the book text. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task.  This task just does this repeatedly
		without ever blocking or delaying. */
		vPrintString( pcTaskName );
	}
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void *pvParameters )
{
TickType_t xLastWakeTime;
const TickType_t xDelay5ms = pdMS_TO_TICKS( 3UL );

	/* The xLastWakeTime variable needs to be initialized with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		vPrintString( "Periodic task is running\r\n" );

		/* We want this task to execute exactly every 10 milliseconds. */
		vTaskDelayUntil( &xLastWakeTime, xDelay5ms );
	}
}



