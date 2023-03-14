#include "exti.h"

void lightInit(void)
{
    Key_Init();

	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_7, GPIO_DIR_OUT); //设置PC8为输出
	GPIO_PullResistorConfig(HT_GPIOB, GPIO_PIN_7,GPIO_PR_UP);
}


