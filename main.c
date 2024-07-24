
/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}


#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500 )

uint32_t ulCallCount;

static void prvOneShotTimerCallback( TimerHandle_t xTimer )
{
	TickType_t xTimeNow;

	/* Obtain the current tick count. */
	xTimeNow = xTaskGetTickCount();

	/* Output a string to show the time at which the callback was executed. */
	console_print( "One-shot timer callback executing %lu \r\n", xTimeNow );

	/* File scope variable. */
	ulCallCount++;
}

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
	TickType_t xTimeNow;

	/* Obtain the current tick count. */
	xTimeNow = xTaskGetTickCount();

	/* Output a string to show the time at which the callback was executed. */
	console_print( "Auto-reload timer callback executing %lu \r\n", xTimeNow );

	ulCallCount++;
}

int main( void )
{
	// Init output console
    console_init();
	TimerHandle_t xAutoReloadTimer, xOneShotTimer;
	BaseType_t xTimer1Started, xTimer2Started;

	/* Create the one shot timer, storing the handle to the created timer in xOneShotTimer. */
	xOneShotTimer = xTimerCreate(
		/* Text name for the software timer - not used by FreeRTOS. */
		"OneShot",
		/* The software timer's period in ticks. */
		mainONE_SHOT_TIMER_PERIOD,
		/* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
		pdFALSE,
		/* This example does not use the timer id. */
		0,
		/* The callback function to be used by the software timer being created. */
		prvOneShotTimerCallback );

	/* Create the auto-reload timer, storing the handle to the created timer in xAutoReloadTimer. */
	xAutoReloadTimer = xTimerCreate(
		/* Text name for the software timer - not used by FreeRTOS. */
		"AutoReload",
		/* The software timer's period in ticks. */
		mainAUTO_RELOAD_TIMER_PERIOD,
		/* Setting uxAutoRealod to pdTRUE creates an auto-reload timer. */
		pdTRUE,
		/* This example does not use the timer id. */
		0,
		/* The callback function to be used by the software timer being created. */
		prvAutoReloadTimerCallback );

	/* Check the software timers were created. */
	if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) )
	{
		/* Start the software timers, using a block time of 0 (no block time). The scheduler has
		not been started yet so any block time specified here would be ignored anyway. */
		xTimer1Started = xTimerStart( xOneShotTimer, 0 );
		xTimer2Started = xTimerStart( xAutoReloadTimer, 0 );
		/* The implementation of xTimerStart() uses the timer command queue, and xTimerStart()
		will fail if the timer command queue gets full. The timer service task does not get
		created until the scheduler is started, so all commands sent to the command queue will
		stay in the queue until after the scheduler has been started. Check both calls to
		xTimerStart() passed. */
		if( ( xTimer1Started == pdPASS ) && ( xTimer2Started == pdPASS ) )
		{
			/* Start the scheduler. */
			vTaskStartScheduler();
		}
	}
	/* As always, this line should not be reached. */
	for( ;; );
}