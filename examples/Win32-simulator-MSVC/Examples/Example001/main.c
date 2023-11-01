/*
 *
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
 *  FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  Full license text is available on the following
 *  link: http://www.freertos.org/a00114.html
 *
 *  http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
 *  the FAQ page "My application does not run, what could be wrong?".  Have you
 *  defined configASSERT()?
 *
 *  http://www.FreeRTOS.org/support - In return for receiving this top quality
 *  embedded software for free we request you assist our global community by
 *  participating in the support forum.
 *
 *  http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 *  including FreeRTOS+Trace - an indispensable productivity tool, a DOS
 *  compatible FAT file system, and our tiny thread aware UDP/IP stack.
 *
 *  http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
 *  Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.
 *
 */

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "supporting_functions.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT    ( 0xffffff )

/* The task functions. */
void vTask1( void * pvParameters );
void vTask2( void * pvParameters );

/*-----------------------------------------------------------*/

int main( void )
{
    /* Create one of the two tasks. */
    xTaskCreate( vTask1,   /* Pointer to the function that implements the task. */
                 "Task 1", /* Text name for the task.  This is to facilitate debugging only. */
                 1000,     /* Stack depth - most small microcontrollers will use much less stack than this. */
                 NULL,     /* We are not using the task parameter. */
                 1,        /* This task will run at priority 1. */
                 NULL );   /* We are not using the task handle. */

    /* Create the other task in exactly the same way. */
    xTaskCreate( vTask2, "Task 2", 1000, NULL, 1, NULL );

    /* Start the scheduler to start the tasks executing. */
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

void vTask1( void * pvParameters )
{
    const char * pcTaskName = "Task 1 is running\r\n";
    volatile uint32_t ul;

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintString( pcTaskName );

        /* Delay for a period. */
        for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
        {
            /* This loop is just a very crude delay implementation.  There is
             * nothing to do in here.  Later exercises will replace this crude
             * loop with a proper delay/sleep function. */
        }
    }
}
/*-----------------------------------------------------------*/

void vTask2( void * pvParameters )
{
    const char * pcTaskName = "Task 2 is running\r\n";
    volatile uint32_t ul;

    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ; ; )
    {
        /* Print out the name of this task. */
        vPrintString( pcTaskName );

        /* Delay for a period. */
        for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
        {
            /* This loop is just a very crude delay implementation.  There is
             * nothing to do in here.  Later exercises will replace this crude
             * loop with a proper delay/sleep function. */
        }
    }
}
