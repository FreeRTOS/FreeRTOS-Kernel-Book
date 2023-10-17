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

/*
 * This file contains some supporting functions that are used by all the
 * examples.  These fall into three categories:
 *
 * 1) IO functions:  vPrintString() and vPrintStringAndNumber().
 *
 * To allow maximum portability the book examples do not rely on any chip
 * specific IO, and instead just output to a console.  However, printing to a
 * console in this manner is not thread safe, so a function is used so the
 * terminal output can be wrapped in a critical section.
 *
 * 2) RTOS hook functions: vApplicationMallocFailedHook(), vApplicationIdleHook()
 * vApplicationIdleHook(), vApplicationStackOverflowHook() and
 * vApplicationTickHook().
 *
 * These are functions that can optionally be defined by the application writer
 * in order to get notifications of events occurring with the executing
 * application.  More information is provided in the comments within the
 * function definitions in this file, and in the book text.
 *
 * 3) configASSERT() implementation: vAssertCalled()
 *
 * The FreeRTOS source code uses an assert() function to trap user and other
 * errors.  configASSERT() is defined in FreeRTOSConfig.h to call
 * vAssertCalled(), which is implemented in this file.  More information is
 * provided in the book text.
 */

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* If this variable is true then pressing a key will end the application.  Some
examples set this to pdFALSE to allow key presses to be used by the
application. */
BaseType_t xKeyPressesStopApplication = pdTRUE;

/*-----------------------------------------------------------*/

void vPrintString( const char *pcString )
{
BaseType_t xKeyHit = pdFALSE;

	/* Print the string, using a critical section as a crude method of mutual
	exclusion. */
	taskENTER_CRITICAL();
	{
		printf( "%s", pcString );
		fflush( stdout );

		/* Allow any key to stop the application. */
		if( xKeyPressesStopApplication == pdTRUE )
		{
			xKeyHit = _kbhit();
		}
	}
	taskEXIT_CRITICAL();

    /* Allow any key to stop the application running.  A real application that
    actually used the key value should protect access to the keyboard too. */
	if( xKeyHit != pdFALSE )
	{
		vTaskEndScheduler();
	}
}
/*-----------------------------------------------------------*/

void vPrintStringAndNumber( const char *pcString, uint32_t ulValue )
{
	/* Print the string, using a critical section as a crude method of mutual
	exclusion. */
	taskENTER_CRITICAL();
	{
		printf( "%s %lu\r\n", pcString, ulValue );
		fflush( stdout );
	}
	taskEXIT_CRITICAL();

	/* Allow any key to stop the application running. */
	if( xKeyPressesStopApplication == pdTRUE )
	{
		if( _kbhit() )
		{
			vTaskEndScheduler();
		}
	}
}
/*-----------------------------------------------------------*/

void vPrintTwoStrings( const char *pcString1, const char *pcString2 )
{
	/* Print the string, using a critical section as a crude method of mutual
	exclusion. */
	vTaskSuspendAll();
	{
		printf( "At time %lu: %s %s\r\n", xTaskGetTickCount(), pcString1, pcString2 );
	}
	xTaskResumeAll();

	/* Allow any key to stop the application running. */
	if( xKeyPressesStopApplication == pdTRUE )
	{
		if( _kbhit() )
		{
			vTaskEndScheduler();
		}
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer, event group, or semaphore is created.  It is also called by various
	parts of the demo application.  If heap_1.c, heap_2.c or heap_4.c are used,
	then the size of the heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains.
	More information is provided in the book text. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

/* An example vApplicationIdleHook() implementation is included here for
completeness, but it is not actually built (it is excluded by the #if 0) as it
is also defined by the examples that actually make use of the function. */
#if 0
	void vApplicationIdleHook( void )
	{
		/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
		to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
		task.  It is essential that code added to this hook function never attempts
		to block in any way (for example, call xQueueReceive() with a block time
		specified, or call vTaskDelay()).  If the application makes use of the
		vTaskDelete() API function then it is also important that
		vApplicationIdleHook() is permitted to return to its calling function,
		because it is the responsibility of the idle task to clean up memory
		allocated by the kernel to any task that has since been deleted.  More
		information is provided in the book text. */
	}
#endif /* 0 */
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is
	called if a stack overflow is detected.  More information is provided in the
	book text. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}
/*-----------------------------------------------------------*/

void vAssertCalled( uint32_t ulLine, const char * const pcFile )
{
/* The following two variables are just to ensure the parameters are not
optimised away and therefore unavailable when viewed in the debugger. */
volatile uint32_t ulLineNumber = ulLine, ulSetNonZeroInDebuggerToReturn = 0;
volatile const char * const pcFileName = pcFile;

	taskENTER_CRITICAL();
	{
		while( ulSetNonZeroInDebuggerToReturn == 0 )
		{
			/* If you want to set out of this function in the debugger to see
			the	assert() location then set ulSetNonZeroInDebuggerToReturn to a
			non-zero value. */
		}
	}
	taskEXIT_CRITICAL();

	/* Remove the potential for compiler warnings issued because the variables
	are set but not subsequently referenced. */
	( void ) pcFileName;
	( void ) ulLineNumber;
}
/*-----------------------------------------------------------*/

