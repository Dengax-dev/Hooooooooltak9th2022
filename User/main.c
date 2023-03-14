#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
#include "usart.h"
#include "delay.h"
#include "dht11.h"
#include "rain.h"
#include "pwm.h"
#include "led.h"
#include "light.h"
#include "esp8266.h"
#include "onenet.h"

#define TIME 100

unsigned char temper = 0,humidity = 0;
int automatic = 1;          //�Զ������Ʊ�־λ
int cloth = 0;              //�·���־λ
int rain = 0;               //��ˮ״̬��־λ
int curtain = 2;            //��������־λ      2-up 3-down
int curtainWifi = 0;        //������wifi��־λ  0-up 1-down
int light = 0;              //��״̬��־λ
int light_con = 0;
int human = 0;              //�˱�־λ
int fan = 0;                //����״̬��־λ
int hungerWifi = 0;         //���¼�״̬        0-up 1-down
int hunger = 2;             //���¼�״̬        2-���� 3-����

unsigned short timeCount = 50; //���ͼ������
unsigned char *dataPtr = NULL; //wifi����

void HardwareInit(void);
void printfInfo(void);

int main(void)
{
	HardwareInit(); //Ӳ����ʼ��

	while(1)
	{
		timeCount ++;
		led_on(); //ϵͳ����״ָ̬ʾ��

		if(timeCount >= TIME) //���ڻ�ȡ��Ϣ
		{
			rain=Rain(); //��ȡ������Ϣ
			cloth = Cloth(); //����״̬
			dht11_read_data(&temper,&humidity); //��ȡ��ʪ������
			
		}

		if((timeCount >= TIME)&&(automatic != 1)) //���Զ�������
		{
			UsartPrintf(USART_DEBUG, "manual mode\r\n");

			if((hungerWifi == 1)&&(hunger == 2)) //�������¼�����
			{
				hungerDown();
				hunger = 3;
			}
			
			if((hungerWifi == 0)&&(hunger == 3))
			{
				hungerUp();
				hunger = 2;
			}

			if((curtainWifi == 1)&&(curtain == 2)) //�������¼�����
			{
				curtainDown(); //��������
				curtain = 3;
			}
			if((curtainWifi == 0)&&(curtain == 3))
			{
				curtainUp();
				curtain = 2;
			}

			printfInfo();

			OneNet_SendData(); //��������
			ESP8266_Clear();

			timeCount = 0;
		}

		if((timeCount >= TIME)&&(automatic == 1)) //�Զ�������
		{
			UsartPrintf(USART_DEBUG, "automatic mode\r\n");
			printfInfo();

			if(cloth == 1)
			{
				if((humidity >= 80)&&(fan == 0)) //ʪ�ȴ���80%������ʪ����
				{
					PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_100); //PWM_CH1 ѡ��PWMͨ��2(Pc8)
					fan = 1;
				}
				if((humidity < 80)&&(fan == 1))
				{
					PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //PWM_CH1 ѡ��PWMͨ��2(Pc8)
					fan = 0;
				}
		
				if(rain&&(curtain == 2)) //������꣬��������
				{
						curtainDown(); //��������
						curtain = 3; //��ֵ��־λ
				} 

				if((!rain)&&(curtain == 3)) //���ʪ��С��80��û�н���
				{
					curtainUp(); //��������
					curtain = 2; //��ֵ��־λ
				}
			}

			OneNet_SendData(); //��������
			ESP8266_Clear();
			
			timeCount = 0;
		}
		
		dataPtr = ESP8266_GetIPD(0); //������������
		if(dataPtr != NULL) OneNet_RevPro(dataPtr);

		led_off(); //ϵͳ����״ָ̬ʾ��
		delay_ms(30);
	}
}


void EXTI4_15_IRQHandler(void) //�жϺ���
{
	extern int light;
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_4);

	if(light == 1)
	{
		if ((light_con == 1)&&(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_4) == 1))
		{
			GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, RESET);
			light_con = 0;
		}
		if ((light_con == 0)&&(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_4) == 0))
		{
			GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, SET);
			light_con = 1;
		}
	}
}

void HardwareInit(void) //���ܳ�ʼ��
{
	delay_init();
	USART1_Configuration();				//����1����ӡ��Ϣ��
	USART2_Configuration();				//����2������ESP8266��
	Rain_Init();
	motorInit(); //���������ʼ��
	cloth_Init();
	led_init();
	PWM_Init();
	dht11_init();			//��Ҫ���ڴ������ú�
	lightInit();
	ESP8266_Init();					//��ʼ��ESP8266

	while(OneNet_DevLink())			//����OneNET
	delay_ms(500);

	PWM_UpdateDuty(STRCAT2(TM_CH_,    0), PWM_DUTY_0); //PWM_CH0 ѡ��PWMͨ��0(Pc4) //������
	PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_0); //PWM_CH1 ѡ��PWMͨ��1(Pc5) //���¼�
	PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //PWM_CH2 ѡ��PWMͨ��2(PC8) //��ʪ����
	// PWM_UpdateDuty(STRCAT2(TM_CH_,    3), PWM_DUTY_100); //PWM_CH3 ѡ��PWMͨ��3(PC9)
	PWM_Cmd(ENABLE);

	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
}

void printfInfo(void)
{
	if(rain==1) printf("raining\r\n\n"); //���������Ϣ
	else printf("no rain\r\n\n");

	printf("temp:%d,humi:%d\r\n\n",temper,humidity); //�����ʪ������

	if(light == 1) printf("light is ON\r\n\n"); //��
	else printf("light is OFF\r\n\n");

	if(curtain == 1) printf("curtain is down\r\n");
	else printf("curtain is up\r\n");

	if(hunger == 1) printf("hunger is down\r\n");
	else printf("hunger is up\r\n");

	if(fan == 1) printf("fan is ON\r\n");
	else printf("fan is OFF\r\n");
}
