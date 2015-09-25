#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "system_stm32f30x.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"

void vFreeRTOSInitGPIO()//on of leds
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void vFreeRTOSInitTIM()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef timer_base;
  TIM_TimeBaseStructInit(&timer_base);
  timer_base.TIM_Prescaler = 72; // частота 72МГц, поэтому один тик таймера - 1мкс
  TIM_TimeBaseInit(TIM1, &timer_base);

  TIM_ICInitTypeDef TIM_ICStructure;
  TIM_ICStructure.TIM_Channel = TIM_Channel_1; // первый канал
  TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // по заднему фронту
  TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // прямо с ножки
  TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // без делителя
  TIM_ICStructure.TIM_ICFilter = 0; // без фильтра

  // эта функция и включает режим PWM input - автоматически настраивает комплементарный канал
  // правда в стандартной библиотеке работает на 1 и 2 канале, на 3 и 4 - не умеет
  TIM_PWMIConfig(TIM1, &TIM_ICStructure);

  /* Выбираем источник для триггера: вход 1 */
  TIM_SelectInputTrigger(TIM1, TIM_TS_TI1FP1);
  /* По событию от триггера счётчик будет сбрасываться. */
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
  /* Включаем события от триггера */
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

  /* Разрешаем таймеру генерировать прерывание по захвату */
  TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
  TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
  /* Включаем таймер */
  TIM_Cmd(TIM1, ENABLE);
  // разрешаем прерывания
  NVIC_EnableIRQ(TIM1_CC_IRQn);
}

void vLedTask(void *pvParameters)//flash of leds
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
  vFreeRTOSInitGPIO();
  vFreeRTOSInitTIM();
  xTaskCreate(vLedTask, "vLedTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  vTaskStartScheduler();
  return 0;
}

void TIM1_IRQHandler()
{
  uint16_t cnt1, cnt2;
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
  {
  	GPIO_SetBits(GPIOE, GPIO_Pin_10);
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    cnt1 = TIM_GetCapture1(TIM1);
    cnt2 = TIM_GetCapture2(TIM1);
  }
}

void vApplicationIdleHook(void) {}

void vApplicationMallocFailedHook(void) { for(;;); }

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{	(void) pcTaskName;
	(void) pxTask;
	for(;;);
}

void vApplicationTickHook(void) {}