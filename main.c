

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook(void)
{
	usleep(15000);
}

#define TIMER_PERIOD pdMS_TO_TICKS(1000)

SemaphoreHandle_t xBinarySemaphore = NULL;
TimerHandle_t xSoftwareTimer = NULL;

static void vHandlerTask(void *pvParameters)
{
	for (;;)
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		console_print("Handler task - 세마포어를 받아 Handler Task를 실행합니다.\r\n\r\n");
	}
}

// The ISR for the software interrupt
static void ulExampleInterruptHandler(TimerHandle_t xSoftwareTimer)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
	console_print("ISR - 인터럽트가 발생했습니다.\r\n");
}

int main(void)
{
	console_init();

	xBinarySemaphore = xSemaphoreCreateBinary();
	if (xBinarySemaphore != NULL)
	{
		xTaskCreate(vHandlerTask, "Handler", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

		xSoftwareTimer = xTimerCreate("Timer", TIMER_PERIOD, pdTRUE, 0, ulExampleInterruptHandler);

		if (xSoftwareTimer != NULL)
		{
			BaseType_t TimerStart = xTimerStart(xSoftwareTimer, 0);
			if (TimerStart == pdPASS)
			{
				console_print("1초마다 발생하는 인터럽트가 생성되었습니다.\r\n");
			}
		}
		vTaskStartScheduler();
	}
	for (;;);
}