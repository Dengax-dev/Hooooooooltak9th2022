#ifndef __EXTI_H
#define __EXTI_H	
 
#include "ht32f5xxxx_01.h"
 
#define EXTI_KEY1_GPIO_ID                (GPIO_PA)
#define EXTI_KEY1_GPIO_PIN               (GPIO_PIN_4)
#define EXTI_KEY1_AFIO_MODE              (AFIO_FUN_GPIO)
#define EXTI_KEY1_CHANNEL				 (4)
 
#define EXTI_KEY2_GPIO_ID                (GPIO_PA)
#define EXTI_KEY2_GPIO_PIN               (GPIO_PIN_1)
#define EXTI_KEY2_AFIO_MODE              (AFIO_FUN_GPIO)
#define EXTI_KEY2_CHANNEL				 (1)
 
#define EXTI_KEY3_GPIO_ID                (GPIO_PA)
#define EXTI_KEY3_GPIO_PIN               (GPIO_PIN_2)
#define EXTI_KEY3_AFIO_MODE              (AFIO_FUN_GPIO)
#define EXTI_KEY3_CHANNEL				 (2)
 
void Key_Init(void);

#endif

