#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "rain.h"
#include "pwm.h"
#include "delay.h"

void Rain_Init(void)
{
  CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PC = 1; //ʹ��PC��//�ٽ�����Ҫ��ʱ���Է��ʳ�Ա��������ʽ������bit���������� ��3*32λ�Ľṹ�� �����³�Ա��Ϊ��Ӧ�����ʱ��λ����һ����ʱ�ӵ����üĴ���д��һ
	CKCU_PeripClockConfig(LEDClock, ENABLE); //һ��Ҫ��ʹ��ʱ�ӣ������������
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_11, GPIO_DIR_IN); //����PC11Ϊ����
	GPIO_PullResistorConfig(HT_GPIOC,GPIO_PIN_11,GPIO_PR_DOWN);//�ڲ���������������,����
	GPIO_InputConfig(HT_GPIOC,GPIO_PIN_11,ENABLE);//����ʹ��
}

void cloth_Init(void)
{
  CKCU_PeripClockConfig_TypeDef ClothClock = {{ 0 }};
	ClothClock.Bit.PC = 1; //ʹ��PC��//�ٽ�����Ҫ��ʱ���Է��ʳ�Ա��������ʽ������bit���������� ��3*32λ�Ľṹ�� �����³�Ա��Ϊ��Ӧ�����ʱ��λ����һ����ʱ�ӵ����üĴ���д��һ
	CKCU_PeripClockConfig(ClothClock, ENABLE); //һ��Ҫ��ʹ��ʱ�ӣ������������
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_12, GPIO_DIR_IN); //����PC12Ϊ����
  GPIO_PullResistorConfig(HT_GPIOC,GPIO_PIN_12,GPIO_PR_DOWN);//�ڲ���������������
  GPIO_InputConfig(HT_GPIOC,GPIO_PIN_12,ENABLE);//����ʹ��
}

//��ˮ��⣺1��������   0�����޽���//
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

//�����⣺1���������� 0����������
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

void motorInit(void) ////���������ʼ��
{
  CKCU_PeripClockConfig_TypeDef MotorClock = {{ 0 }};
	MotorClock.Bit.PC = 1; //ʹ��PC
	CKCU_PeripClockConfig(MotorClock, ENABLE); //һ��Ҫ��ʹ��ʱ�ӣ������������
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_OUT); //����PC15Ϊ���
}


/////////////////////////////////////////////////////////////////////////////////////////////////

int i;

void curtainDown(void) //��������
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


void curtainUp(void) //��������
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

void hungerUp(void) //�������¼�
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


void hungerDown(void) //�������¼�
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

