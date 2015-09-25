#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "system_stm32f30x.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"

uint32_t cnt1=0, cnt2=0;

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
{ /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* TIM2 chennel2 configuration : PA.01 */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Connect TIM pin to AF1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_1);
  /* Enable the TIM2 global Interrupt */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);
  /* Select the TIM2 Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);
  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);
  /* Enable the CC2 Interrupt Request */
  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}

void vLedTask(void *pvParameters)//flash of leds
{
	while(1)
	{
		GPIOE->ODR ^= GPIO_Pin_8;
		vTaskDelay(1000);
    GPIO_ResetBits(GPIOE, GPIO_Pin_10);
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

void TIM2_IRQHandler()
{
  GPIO_SetBits(GPIOE, GPIO_Pin_10);
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

  cnt1 = TIM_GetCapture1(TIM2);
  cnt2 = TIM_GetCapture2(TIM2);
}

void vApplicationIdleHook(void) {}

void vApplicationMallocFailedHook(void) { for(;;); }

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{	(void) pcTaskName;
	(void) pxTask;
	for(;;);
}

void vApplicationTickHook(void) {}