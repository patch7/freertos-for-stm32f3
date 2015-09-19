#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "system_stm32f30x.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"

void vFreeRTOSInitRCC()
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
}

void vFreeRTOSInitGPIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void vLedTask(void *pvParameters)
{
	while(1)
	{
		GPIOE->ODR ^= GPIO_Pin_8;
		vTaskDelay(50);
	}
  vTaskDelete(NULL);
}

int main()
{
  vFreeRTOSInitRCC();
  vFreeRTOSInitGPIO();
  xTaskCreate(vLedTask, "vLedTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  vTaskStartScheduler();
  return 0;
}

void vApplicationIdleHook(void)
{}

void vApplicationMallocFailedHook(void)
{
	for(;;);
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	for(;;);
}

void vApplicationTickHook(void)
{}