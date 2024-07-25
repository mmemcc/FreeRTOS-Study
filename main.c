

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook(void)
{
	usleep(15000);
}

SemaphoreHandle_t xMutex;

static void PrintTask1(void *pvParameters)
{
	char *pcStringToPrint;
	pcStringToPrint = (char *)pvParameters;
	for (;;)
	{
		xSemaphoreTake(xMutex, portMAX_DELAY);

		console_print("%s", pcStringToPrint);
		fflush(stdout);
		vTaskDelay(pdMS_TO_TICKS(1000));

		xSemaphoreGive(xMutex);
	}
}

static void PrintTask2(void *pvParameters)
{
	const char *pcStringToPrint;
	pcStringToPrint = (char *)pvParameters;
	for (;;)
	{
		xSemaphoreTake(xMutex, portMAX_DELAY);
		console_print("%s", pcStringToPrint);
		xSemaphoreGive(xMutex);
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

int main(void)
{
	console_init();

	xMutex = xSemaphoreCreateMutex();
	if (xMutex != NULL)
	{
		xTaskCreate(PrintTask1, "Print1", configMINIMAL_STACK_SIZE, "Task 1 ***************************************\r\n", 1, NULL);
		xTaskCreate(PrintTask2, "Print2", configMINIMAL_STACK_SIZE, "Task 2 ---------------------------------------\r\n", 2, NULL);

		vTaskStartScheduler();
	}
	for (;;);
}