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
 * The callback functions used by the one-shot and auto-reload timers
 * respectively.
 */
static void prvOneShotTimerCallback( TimerHandle_t xTimer );
static void prvAutoReloadTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

int main( void )
{
    TimerHandle_t xAutoReloadTimer, xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;

    /* Create the one shot software timer, storing the handle to the created
     * software timer in xOneShotTimer. */
    xOneShotTimer = xTimerCreate( "OneShot",                 /* Text name for the software timer - not used by FreeRTOS. */
                                  mainONE_SHOT_TIMER_PERIOD, /* The software timer's period in ticks. */
                                  pdFALSE,                   /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
                                  0,                         /* This example does not use the timer id. */
                                  prvOneShotTimerCallback ); /* The callback function to be used by the software timer being created. */

    /* Create the auto-reload software timer, storing the handle to the created
     * software timer in xAutoReloadTimer. */
    xAutoReloadTimer = xTimerCreate( "AutoReload",                 /* Text name for the software timer - not used by FreeRTOS. */
                                     mainAUTO_RELOAD_TIMER_PERIOD, /* The software timer's period in ticks. */
                                     pdTRUE,                       /* Set uxAutoRealod to pdTRUE to create an auto-reload software timer. */
                                     0,                            /* This example does not use the timer id. */
                                     prvAutoReloadTimerCallback ); /* The callback function to be used by the software timer being created. */

    /* Check the timers were created. */
    if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
    {
        /* Start the software timers, using a block time of 0 (no block time).
         * The scheduler has not been started yet so any block time specified here
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

static void prvOneShotTimerCallback( TimerHandle_t xTimer )
{
    static TickType_t xTimeNow;

    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();

    /* Output a string to show the time at which the callback was executed. */
    vPrintStringAndNumber( "One-shot timer callback executing", xTimeNow );
}
/*-----------------------------------------------------------*/

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
    static TickType_t xTimeNow;

    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();

    /* Output a string to show the time at which the callback was executed. */
    vPrintStringAndNumber( "Auto-reload timer callback executing", xTimeNow );
}
/*-----------------------------------------------------------*/
