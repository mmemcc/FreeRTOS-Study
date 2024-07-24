

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook( void )
{
    usleep( 15000 );
}

QueueHandle_t xQueue;

static void vSenderTask( void *pvParameters )
{
	int32_t lValueToSend;
	BaseType_t xStatus;
	lValueToSend = ( int32_t ) pvParameters;
	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
		if( xStatus != pdPASS )
		{
			console_print( "Could not send to the queue.\r\n" );
		}
		else
		{
			console_print( "Sent = %d\r\n", lValueToSend );
		}
		
		vTaskDelay( pdMS_TO_TICKS( 1000 ) );
	}
}

static void vReceiverTask( void *pvParameters )
{
	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
	UBaseType_t uxQueueLength;
	for( ;; )
	{
		uxQueueLength = uxQueueMessagesWaiting( xQueue );
		console_print( "현재 queue에 기록된 데이타 수: %u\r\n", uxQueueLength );

		xStatus = xQueueReceive( xQueue, &lReceivedValue, 0 );
		if( xStatus == pdPASS )
			{
			console_print( "Received = %d\r\n", lReceivedValue );
			}
		else
		{
			console_print( "Could not receive from the queue.\r\n" );
		}

		vTaskDelay( pdMS_TO_TICKS( 500 ) );
	}
}

int main( void )
{
    console_init(); 

	xQueue = xQueueCreate( 5, sizeof( int32_t ) );
	if( xQueue != NULL )
	{
		xTaskCreate( vSenderTask, "Sender1", configMINIMAL_STACK_SIZE, ( void * ) 100, 1, NULL );
		xTaskCreate( vSenderTask, "Sender2", configMINIMAL_STACK_SIZE, ( void * ) 200, 1, NULL );
		xTaskCreate( vReceiverTask, "Receiver", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
		vTaskStartScheduler();
	}
	else
	{
	}
	for( ;; );
}