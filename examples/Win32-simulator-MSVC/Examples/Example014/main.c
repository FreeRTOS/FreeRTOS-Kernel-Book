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
#include "timers.h"

/* Demo includes. */
#include "supporting_functions.h"

/* The periods assigned to the one-shot and auto-reload timers respectively. */
#define mainONE_SHOT_TIMER_PERIOD       ( pdMS_TO_TICKS( 3333UL ) )
#define mainAUTO_RELOAD_TIMER_PERIOD    ( pdMS_TO_TICKS( 500UL ) )

/*-----------------------------------------------------------*/

/*
 * The callback function that is used by both timers.
 */
static void prvTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

/* The timer handles are used inside the callback function so this time are
 * given file scope. */
static TimerHandle_t xAutoReloadTimer, xOneShotTimer;

int main( void )
{
    BaseType_t xTimer1Started, xTimer2Started;

    /* Create the one shot timer, storing the handle to the created timer in
     * xOneShotTimer. */
    xOneShotTimer = xTimerCreate( "OneShot",                 /* Text name for the timer - not used by FreeRTOS. */
                                  mainONE_SHOT_TIMER_PERIOD, /* The timer's period in ticks. */
                                  pdFALSE,                   /* Set uxAutoRealod to pdFALSE to create a one-shot timer. */
                                  0,                         /* The timer ID is initialised to 0. */
                                  prvTimerCallback );        /* The callback function to be used by the timer being created. */

    /* Create the auto-reload, storing the handle to the created timer in
     * xAutoReloadTimer. */
    xAutoReloadTimer = xTimerCreate( "AutoReload",                 /* Text name for the timer - not used by FreeRTOS. */
                                     mainAUTO_RELOAD_TIMER_PERIOD, /* The timer's period in ticks. */
                                     pdTRUE,                       /* Set uxAutoRealod to pdTRUE to create an auto-reload timer. */
                                     0,                            /* The timer ID is initialised to 0. */
                                     prvTimerCallback );           /* The callback function to be used by the timer being created. */

    /* Check the timers were created. */
    if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
    {
        /* Start the timers, using a block time of 0 (no block time).  The
         * scheduler has not been started yet so any block time specified here
         * would be ignored anyway. */
        xTimer1Started = xTimerStart( xOneShotTimer, 0 );
        xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );

        /* The implementation of xTimerStart() uses the timer command queue, and
         * xTimerStart() will fail if the timer command queue gets full.  The timer
         * service task does not get created until the scheduler is started, so all
         * commands sent to the command queue will stay in the queue until after
         * the scheduler has been started.  Check both calls to xTimerStart()
         * passed. */
        if( ( xTimer1Started == pdPASS ) && ( xTimer2Started == pdPASS ) )
        {
            /* Start the scheduler. */
            vTaskStartScheduler();
        }
    }

    /* If the scheduler was started then the following line should never be
     * reached because vTaskStartScheduler() will only return if there was not
     * enough FreeRTOS heap memory available to create the Idle and (if configured)
     * Timer tasks.  Heap management, and techniques for trapping heap exhaustion,
     * are described in the book text. */
    for( ; ; )
    {
    }

    return 0;
}
/*-----------------------------------------------------------*/

static void prvTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow;
    uint32_t ulExecutionCount;

    /* The count of the number of times this software timer has expired is
     * stored in the timer's ID.  Obtain the ID, increment it, then save it as the
     * new ID value.  The ID is a void pointer, so is cast to a uint32_t. */
    ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    ulExecutionCount++;
    vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );

    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();

    /* The handle of the one-shot timer was stored in xOneShotTimer when the
     * timer was created.  Compare the handle passed into this function with
     * xOneShotTimer to determine if it was the one-shot or auto-reload timer that
     * expired, then output a string to show the time at which the callback was
     * executed. */
    if( xTimer == xOneShotTimer )
    {
        vPrintStringAndNumber( "One-shot timer callback executing", xTimeNow );
    }
    else
    {
        /* xTimer did not equal xOneShotTimer, so it must be the auto-reload
         * timer that expired. */
        vPrintStringAndNumber( "Auto-reload timer callback executing", xTimeNow );

        if( ulExecutionCount == 5 )
        {
            /* Stop the auto-reload timer after it has executed 5 times.  This
             * callback function executes in the context of the RTOS daemon task so
             * must not call any functions that might place the daemon task into
             * the Blocked state.  Therefore a block time of 0 is used. */
            xTimerStop( xTimer, 0 );
        }
    }
}
/*-----------------------------------------------------------*/
