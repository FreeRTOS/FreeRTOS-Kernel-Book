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

/* Standard includes. */
#include <conio.h>

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Demo includes. */
#include "supporting_functions.h"

/* The periods assigned to the one-shot timer. */
#define mainBACKLIGHT_TIMER_PERIOD		( pdMS_TO_TICKS( 5000UL ) )

/*-----------------------------------------------------------*/

/*
 * The callback function used by the timer.
 */
static void prvBacklightTimerCallback( TimerHandle_t xTimer );

/*
 * A real application, running on a real target, would probably read button
 * pushes in an interrupt.  That allows the application to be event driven, and
 * prevents CPU time being wasted by polling for key presses when no keys have
 * been pressed.  It is not practical to use real interrupts when using the
 * FreeRTOS Windows port, so the vKeyHitTask() task is created to provide the
 * key reading functionality by simply polling the keyboard.
 */
static void vKeyHitTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* This example does not have a real backlight to turn on and off, so the
following variable is used to just hold the state of the backlight. */
static BaseType_t xSimulatedBacklightOn = pdFALSE;

/* The software timer used to turn the backlight off. */
static TimerHandle_t xBacklightTimer = NULL;

/*-----------------------------------------------------------*/

int main( void )
{
	/* The backlight is off at the start. */
	xSimulatedBacklightOn = pdFALSE;

	/* Create the one shot timer, storing the handle to the created timer in
	xOneShotTimer. */
	xBacklightTimer = xTimerCreate( "Backlight",				/* Text name for the timer - not used by FreeRTOS. */
									mainBACKLIGHT_TIMER_PERIOD,	/* The timer's period in ticks. */
									pdFALSE,					/* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
									0,							/* The timer ID is not used in this example. */
									prvBacklightTimerCallback );/* The callback function to be used by the timer being created. */

	/* A real application, running on a real target, would probably read button
	pushes in an interrupt.  That allows the application to be event driven, and
	prevents CPU time being wasted by polling for key presses when no keys have
	been pressed.  It is not practical to use real interrupts when using the
	FreeRTOS Windows port, so the vKeyHitTask() task is created to instead 
	provide the	key reading functionality by simply polling the keyboard. */
	xTaskCreate( vKeyHitTask, "Key poll", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	/* Start the timer. */
	xTimerStart( xBacklightTimer, 0 );

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* As in previous examples, vTaskStartScheduler() shoudl not return so the
	following lines should not be reached. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

static void prvBacklightTimerCallback( TimerHandle_t xTimer )
{
TickType_t xTimeNow = xTaskGetTickCount();

	/* The backlight timer expired, turn the backlight off. */
	xSimulatedBacklightOn = pdFALSE;

	/* Print the time at which the backlight was turned off. */
	vPrintStringAndNumber( "Timer expired, turning backlight OFF at time\t", xTimeNow );
}
/*-----------------------------------------------------------*/

static void vKeyHitTask( void *pvParameters )
{
const TickType_t xShortDelay = pdMS_TO_TICKS( 50 );
extern BaseType_t xKeyPressesStopApplication;
TickType_t xTimeNow;

	/* This example uses key presses, so prevent key presses being used to end
	the application. */
	xKeyPressesStopApplication = pdFALSE;

	vPrintString( "Press a key to turn the backlight on.\r\n" );

	/* A real application, running on a real target, would probably read button
	pushes in an interrupt.  That allows the application to be event driven, and
	prevents CPU time being wasted by polling for key presses when no keys have
	been pressed.  It is not practical to use real interrupts when using the
	FreeRTOS Windows port, so this task is created to instead provide the key 
	reading functionality by simply polling the keyboard. */
	for( ;; )
	{
		/* Has a key been pressed? */
		if( _kbhit() != 0 )
		{
			/* Record the time at which the key press was noted. */
			xTimeNow = xTaskGetTickCount();

			/* A key has been pressed. */
			if( xSimulatedBacklightOn == pdFALSE )
			{
				/* The backlight was off so turn it on and print the time at
				which it was turned on. */
				xSimulatedBacklightOn = pdTRUE;
				vPrintStringAndNumber( "Key pressed, turning backlight ON at time\t", xTimeNow );
			}
			else
			{
				/* The backlight was already on so print a message to say the
				backlight is about to be reset and the time at which it was
				reset. */
				vPrintStringAndNumber( "Key pressed, resetting software timer at time\t", xTimeNow );
			}

			/* Reset the software timer.  If the backlight was previously off
			this call will start the timer.  If the backlight was previously on
			this call will restart the timer.  A real application will probably
			read key presses in an interrupt.  If this function was an interrupt
			service routine then xTimerResetFromISR() must be used instead of
			xTimerReset(). */
			xTimerReset( xBacklightTimer, xShortDelay );

			/* Read and discard the key that was pressed. */
			( void ) _getch();
		}

		/* Don't poll too quickly. */
		vTaskDelay( xShortDelay );
	}
}
/*-----------------------------------------------------------*/

