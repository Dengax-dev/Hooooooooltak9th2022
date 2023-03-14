#include "exti.h"
#include "ht32.h"
#include "delay.h"
#include "ht32_board.h"

void Key_Init(void)
{
	// CKCU_PeripClockConfig_TypeDef LClock = {{ 0 }};
	// LClock.Bit.PC = 1;

	EXTI_InitTypeDef EXTI_InitStruct;
	
	CKCU_PeripClockConfig_TypeDef CKCUClock= {{0}};
	CKCUClock.Bit.PB = 1;  //开启GPIOB时钟
	CKCUClock.Bit.AFIO = 1; 
	CKCUClock.Bit.EXTI = 1;  //开启中断时钟
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_InputConfig(HT_GPIOB, GPIO_PIN_4, ENABLE);
  	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_4, GPIO_PR_DOWN);


	AFIO_GPxConfig( GPIO_PB, AFIO_PIN_4, AFIO_FUN_GPIO);
  	AFIO_EXTISourceConfig(AFIO_EXTI_CH_4, AFIO_ESS_PB);//中断来源选择PB4
	
	EXTI_InitStruct.EXTI_Channel=EXTI_CHANNEL_4;
	EXTI_InitStruct.EXTI_Debounce=EXTI_DEBOUNCE_ENABLE;//决定是否去抖
	EXTI_InitStruct.EXTI_DebounceCnt = 65535*100;     
	EXTI_InitStruct.EXTI_IntType=EXTI_BOTH_EDGE;  //双边沿触发
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_EnableIRQ(EXTI4_15_IRQn);//	
	EXTI_IntConfig(EXTI_CHANNEL_4, ENABLE);//检测中断源状态函数

}

/* void EXTI4_15_IRQHandler(void)
{
		EXTI_ClearEdgeFlag(EXTI_CHANNEL_4);

    	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_8, RESET);
		// delay_ms(1000);
		// if((HT_GPIOB, GPIO_PIN_4) == SET)
		// {
		// GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_8, SET);
		// }
		// else GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_8, SET);
		
} */

