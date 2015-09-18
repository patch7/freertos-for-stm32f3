#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "system_stm32f30x.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"

void main()
{
  ;
}

void vApplicationIdleHook(void)
{}

void vApplicationMallocFailedHook(void)
{}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) pxTask;

	for(;;);
}

void vApplicationTickHook(void)
{}