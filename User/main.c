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
int automatic = 1;          //自动化控制标志位
int cloth = 0;              //衣服标志位
int rain = 0;               //雨水状态标志位
int curtain = 2;            //防雨帘标志位      2-up 3-down
int curtainWifi = 0;        //防雨帘wifi标志位  0-up 1-down
int light = 0;              //灯状态标志位
int light_con = 0;
int human = 0;              //人标志位
int fan = 0;                //风扇状态标志位
int hungerWifi = 0;         //晾衣架状态        0-up 1-down
int hunger = 2;             //晾衣架状态        2-升起 3-放下

unsigned short timeCount = 50; //发送间隔变量
unsigned char *dataPtr = NULL; //wifi数据

void HardwareInit(void);
void printfInfo(void);

int main(void)
{
	HardwareInit(); //硬件初始化

	while(1)
	{
		timeCount ++;
		led_on(); //系统运行状态指示灯

		if(timeCount >= TIME) //定期获取信息
		{
			rain=Rain(); //获取降雨信息
			cloth = Cloth(); //衣物状态
			dht11_read_data(&temper,&humidity); //获取温湿度数据
			
		}

		if((timeCount >= TIME)&&(automatic != 1)) //非自动化控制
		{
			UsartPrintf(USART_DEBUG, "manual mode\r\n");

			if((hungerWifi == 1)&&(hunger == 2)) //控制晾衣架升降
			{
				hungerDown();
				hunger = 3;
			}
			
			if((hungerWifi == 0)&&(hunger == 3))
			{
				hungerUp();
				hunger = 2;
			}

			if((curtainWifi == 1)&&(curtain == 2)) //控制晾衣架升降
			{
				curtainDown(); //放下帘子
				curtain = 3;
			}
			if((curtainWifi == 0)&&(curtain == 3))
			{
				curtainUp();
				curtain = 2;
			}

			printfInfo();

			OneNet_SendData(); //发送数据
			ESP8266_Clear();

			timeCount = 0;
		}

		if((timeCount >= TIME)&&(automatic == 1)) //自动化控制
		{
			UsartPrintf(USART_DEBUG, "automatic mode\r\n");
			printfInfo();

			if(cloth == 1)
			{
				if((humidity >= 80)&&(fan == 0)) //湿度大于80%开启除湿风扇
				{
					PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_100); //PWM_CH1 选择PWM通道2(Pc8)
					fan = 1;
				}
				if((humidity < 80)&&(fan == 1))
				{
					PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //PWM_CH1 选择PWM通道2(Pc8)
					fan = 0;
				}
		
				if(rain&&(curtain == 2)) //如果下雨，放下帘子
				{
						curtainDown(); //放下帘子
						curtain = 3; //赋值标志位
				} 

				if((!rain)&&(curtain == 3)) //如果湿度小于80，没有降雨
				{
					curtainUp(); //升起帘子
					curtain = 2; //赋值标志位
				}
			}

			OneNet_SendData(); //发送数据
			ESP8266_Clear();
			
			timeCount = 0;
		}
		
		dataPtr = ESP8266_GetIPD(0); //接收网络数据
		if(dataPtr != NULL) OneNet_RevPro(dataPtr);

		led_off(); //系统运行状态指示灯
		delay_ms(30);
	}
}


void EXTI4_15_IRQHandler(void) //中断函数
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

void HardwareInit(void) //功能初始化
{
	delay_init();
	USART1_Configuration();				//串口1，打印信息用
	USART2_Configuration();				//串口2，驱动ESP8266用
	Rain_Init();
	motorInit(); //电机辅助初始化
	cloth_Init();
	led_init();
	PWM_Init();
	dht11_init();			//需要放在串口配置后
	lightInit();
	ESP8266_Init();					//初始化ESP8266

	while(OneNet_DevLink())			//接入OneNET
	delay_ms(500);

	PWM_UpdateDuty(STRCAT2(TM_CH_,    0), PWM_DUTY_0); //PWM_CH0 选择PWM通道0(Pc4) //防雨帘
	PWM_UpdateDuty(STRCAT2(TM_CH_,    1), PWM_DUTY_0); //PWM_CH1 选择PWM通道1(Pc5) //晾衣架
	PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //PWM_CH2 选择PWM通道2(PC8) //除湿风扇
	// PWM_UpdateDuty(STRCAT2(TM_CH_,    3), PWM_DUTY_100); //PWM_CH3 选择PWM通道3(PC9)
	PWM_Cmd(ENABLE);

	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
}

void printfInfo(void)
{
	if(rain==1) printf("raining\r\n\n"); //输出下雨信息
	else printf("no rain\r\n\n");

	printf("temp:%d,humi:%d\r\n\n",temper,humidity); //输出温湿度数据

	if(light == 1) printf("light is ON\r\n\n"); //灯
	else printf("light is OFF\r\n\n");

	if(curtain == 1) printf("curtain is down\r\n");
	else printf("curtain is up\r\n");

	if(hunger == 1) printf("hunger is down\r\n");
	else printf("hunger is up\r\n");

	if(fan == 1) printf("fan is ON\r\n");
	else printf("fan is OFF\r\n");
}
