#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "rain.h"
#include "pwm.h"
#include "delay.h"

void Rain_Init(void)
{
  CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PC = 1; //使能PC，//再将所需要的时钟以访问成员变量的形式，其中bit是联合体内 含3*32位的结构体 ，其下成员均为对应外设的时钟位，置一即该时钟的配置寄存器写入一
	CKCU_PeripClockConfig(LEDClock, ENABLE); //一定要先使能时钟，再配置下面的
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_11, GPIO_DIR_IN); //设置PC11为输入
	GPIO_PullResistorConfig(HT_GPIOC,GPIO_PIN_11,GPIO_PR_DOWN);//内部上下拉电阻配置,下拉
	GPIO_InputConfig(HT_GPIOC,GPIO_PIN_11,ENABLE);//输入使能
}

void cloth_Init(void)
{
  CKCU_PeripClockConfig_TypeDef ClothClock = {{ 0 }};
	ClothClock.Bit.PC = 1; //使能PC，//再将所需要的时钟以访问成员变量的形式，其中bit是联合体内 含3*32位的结构体 ，其下成员均为对应外设的时钟位，置一即该时钟的配置寄存器写入一
	CKCU_PeripClockConfig(ClothClock, ENABLE); //一定要先使能时钟，再配置下面的
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_12, GPIO_DIR_IN); //设置PC12为输入
  GPIO_PullResistorConfig(HT_GPIOC,GPIO_PIN_12,GPIO_PR_DOWN);//内部上下拉电阻配置
  GPIO_InputConfig(HT_GPIOC,GPIO_PIN_12,ENABLE);//输入使能
}

//雨水检测：1——降雨   0——无降雨//
int Rain(void)
{
  if((GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_11) == RESET))
  {
    return 1;
  }
  else 
  {
    return 0;
  }
}

//衣物检测：1——有衣物 0——无衣物
int Cloth(void)
{
  if((GPIO_ReadInBit(HT_GPIOC, GPIO_PIN_12) == RESET))
  {
    return 0;
  }
  else 
  {
    return 1;
  }
}

void motorInit(void) ////电机辅助初始化
{
  CKCU_PeripClockConfig_TypeDef MotorClock = {{ 0 }};
	MotorClock.Bit.PC = 1; //使能PC
	CKCU_PeripClockConfig(MotorClock, ENABLE); //一定要先使能时钟，再配置下面的
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_OUT); //设置PC15为输出
}


/////////////////////////////////////////////////////////////////////////////////////////////////

int i;

void curtainDown(void) //放下帘子
{
  GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_0, RESET);
  for(i=0;i<=100;i++)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    0), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }

  delay_ms(1500);
  
  for(i=100;i>0;i--)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    0), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }
  PWM_UpdateDuty(STRCAT2(TM_CH_,    0), PWM_DUTY_0);
}


void curtainUp(void) //升起帘子
{
  GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_0, SET);
  for(i=100;i > 0;i--)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    0), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }

  delay_ms(1500);

  for(i=0;i <=100;i++)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    0), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }
  PWM_UpdateDuty(STRCAT2(TM_CH_,    0), PWM_DUTY_100);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void hungerUp(void) //升起晾衣架
{
  GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_1, RESET);
  PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_100);

  for(i=0;i <= 70;i++)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    1), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }

  delay_ms(600);

  for(i=70;i > 0;i--)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    1), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }
  PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_0);
}


void hungerDown(void) //放下晾衣架
{
  GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_1, SET);
  // PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_100);

  for(i=100;i > 30;i--)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    1), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }

  delay_ms(100);

  for(i=30;i < 100;i++)
  {
    PWM_UpdateDuty(STRCAT2(TM_CH_,    1), HTCFG_PWM_TM_RELOAD * i/100);
    delay_ms(20);
  }
  PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_100);
}

