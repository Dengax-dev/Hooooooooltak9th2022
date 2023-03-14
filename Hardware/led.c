
#include "led.h"

void led_init(void)
{
	CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PC = 1; //使能PC
	CKCU_PeripClockConfig(LEDClock, ENABLE); //一定要先使能时钟，再配置下面的
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_15|GPIO_PIN_14, GPIO_DIR_OUT); //设置PC15为输出
	// led_off();
}

void led_on(void)
{
   GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_15, RESET);
	 GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_14, SET);
}

void led_off(void)
{
   GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_15, SET);
	 GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_14, RESET);
	
}






