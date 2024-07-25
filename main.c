

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "timers.h"

/* Local includes. */
#include "console.h" // posix 환경에서 console을 사용하기 위한 헤더파일. printf 대신 사용

// Simulation of the CPU hardware sleeping mode
// Idle task hook, 지우면 안됨
void vApplicationIdleHook(void)
{
	usleep(15000);
}

// 이벤트 그룹 핸들러
EventGroupHandle_t xEventGroup;

// 비트 플래그 정의
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

void vTaskWaitBit1(void *pvParameters)
{
	EventBits_t uxBits;

	for (;;)
	{
		uxBits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_1, pdPASS, pdTRUE, portMAX_DELAY);
		console_print("Bit 0과 Bit 1가 설정되어 Wait Task 1을 수행합니다.\n");
	}
}
void vTaskWaitBit2(void *pvParameters)
{
	EventBits_t uxBits;

	for (;;)
	{
		uxBits = xEventGroupWaitBits(xEventGroup, BIT_0 | BIT_1 | BIT_2, pdPASS, pdTRUE, portMAX_DELAY);
		console_print("모든 Bit가 설정되어 Wait Task 2을 수행합니다.\n");
	}
} 

void vTaskSetBit1(void *pvParameters)
{
	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		// 비트 플래그 0과 1 설정
		EventBits_t uxBits = xEventGroupSetBits(xEventGroup, BIT_0 | BIT_1);
		console_print("vTaskSetBit1 - Set BIT_0 and BIT_1\r\n");
		
	}
}

void vTaskSetBit2(void *pvParameters)
{
	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(2000));
		// 비트 플래그 2 설정
		EventBits_t uxBits = xEventGroupSetBits(xEventGroup, BIT_2);
		console_print("vTaskSetBit2 - Set BIT_2\r\n");
		
	}
}
int main(void)
{
	console_init();

	xEventGroup = xEventGroupCreate();

	xTaskCreate(vTaskWaitBit1, "TaskWaitBit1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTaskWaitBit2, "TaskWaitBit2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTaskSetBit1, "vTaskSetBit1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTaskSetBit2, "vTaskSetBit2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();

	for (;;)
		;
}