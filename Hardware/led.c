
#include "led.h"

void led_init(void)
{
	CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PC = 1; //ʹ��PC
	CKCU_PeripClockConfig(LEDClock, ENABLE); //һ��Ҫ��ʹ��ʱ�ӣ������������
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_15|GPIO_PIN_14, GPIO_DIR_OUT); //����PC15Ϊ���
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






