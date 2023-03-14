#include "ht32.h"
#include "ht32_board.h"
#include "dht11.h"
#include "delay.h"
#include "usart.h"

	// USART1_Configuration();				//串口1，打印信息用


void dht11_init(void)
{
	CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PB = 1; //使能PC，//再将所需要的时钟以访问成员变量的形式，其中bit是联合体内 含3*32位的结构体 ，其下成员均为对应外设的时钟位，置一即该时钟的配置寄存器写入一
	CKCU_PeripClockConfig(LEDClock, ENABLE); //一定要先使能时钟，再配置下面的
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //设置PC1为输出

    DHT11_Rst();
	// DHT11_Check();
    UsartPrintf(USART_DEBUG, "check DHT11(0--success):%d\r\n",DHT11_Check());
	
}

void DHT11_Rst(void)	
{
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //设置PC1为输出
    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, RESET);
    delay_ms(30);     //主机发送开始信号（拉低），拉低至少18ms，我们拉低30
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //设置PC1为输出
    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, SET);  //PC1控制DHT11
    delay_us(30);     //主机发送开始信号（拉高），主机拉高20~40us，我们拉高40us
    // GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, SET);
    
    // UsartPrintf(USART_DEBUG, "reset success\r\n");
}


/*-------------------------------------------------*/
/*函数名：等待DHT11的回应                          */
/*参  数：无                                       */
/*返回值：1错误 0正确                              */
/*-------------------------------------------------*/ 
char DHT11_Check(void) 	   
{   
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //设置PC1为输入
	GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//内部上下拉电阻配置,上拉
	GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//输入使能
	// delay_us(20);
	char timeout1,timeout2;                            //定义一个变量用于超时判断  
	timeout1 = 0;                             //超时变量清零    
	timeout2 =0;
    while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout1 < 70))//DHT11会拉低40~50us,我们等待70us超时时间	
	{	 
		timeout1++;                           //超时变量+1
		delay_us(1);                       	 //延时1us
	} 

	if(timeout1 >= 70)return 1;               //如果timeout>=100,说明是因为超时退出的while循环，返回1表示错误
	// else timeout = 0;                        //反之，说明是因为等到了DHT11拉低IO，退出的while循环，正确并清零timeout
    while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout2 < 70))//DHT11拉低后会再次拉高40~50us,,我们等待70us超时时间	
	{ 		
		timeout2++;                           //超时变量+1
		delay_us(1);                          
	}
	// UsartPrintf(USART_DEBUG, "timeout1;%d\r\n",timeout1);
	// UsartPrintf(USART_DEBUG, "timeout2;%d\r\n",timeout2);

	if(timeout2 >= 70)return 2;               //如果timeout>=70,说明是因为超时退出的while循环，返回2表示错误  
	return 0;                                //反之正确，返回0
}

/*-------------------------------------------------*/
/*函数名：读取一个位                               */
/*参  数：无                                       */
/*返回值：1或0                                     */
/*-------------------------------------------------*/ 

char DHT11_Read_Bit(void) 			 
{
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //设置PC1为输入
	// GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//内部上下拉电阻配置
	// GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//输入使能

 	char timeout;                          	   //定义一个变量用于超时判断  
	
	timeout = 0;                               //清零timeout	
	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout < 40))//每一位数据开始，是12~14us的低电平，我们等40us
	{   
		timeout++;                             //超时变量+1
		delay_us(1);                            
	}
	timeout = 0;                               //清零timeout	
	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout < 60))//接下来，DHT11会拉高IO，根据拉高的时间判断是0或1，我们等60us
	{  
		timeout++;                             //超时变量+1
		delay_us(1);                            
	}

	// delay_us(30); 
	delay_us(30);                              
	
	if(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET)return 1;                   //如果延时后，是高电平，那么本位接收的是1，返回1
	else return 0;		                       //反之延时后，是低电平，那么本位接收的是0，返回0
	// delay_us(30); 
}

// char DHT11_Read_Bit(void) 			 
// {
// 	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //设置PC1为输入
// 	// GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//内部上下拉电阻配置
// 	// GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//输入使能

//  	char timeout;                          	   //定义一个变量用于超时判断  
	
// 	timeout = 0;                               //清零timeout	
// 	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout < 40))//每一位数据开始，是12~14us的低电平，我们等40us
// 	{   
// 		timeout++;                             //超时变量+1
// 		delay_us(1);                            
// 	}
// 	timeout = 0;                               //清零timeout	
// 	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout < 60))//接下来，DHT11会拉高IO，根据拉高的时间判断是0或1，我们等60us
// 	{  
// 		timeout++;                             //超时变量+1
// 		delay_us(1);                            
// 	}
// 	delay_us(35);                               
// 	if(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET)return 1;                   //如果延时后，是高电平，那么本位接收的是1，返回1
// 	else return 0;		                       //反之延时后，是低电平，那么本位接收的是0，返回0
// 	// delay_us(30); 
// }

/*-------------------------------------------------*/
/*函数名：读取一个字节                              */
/*参  数：无                                       */
/*返回值：数据                                      */
/*-------------------------------------------------*/ 
char DHT11_Read_Byte(void)    
{        
    char i;                       			  //定义一个变量用于for循环  
	char dat;                              	  //定义一个变量用于保存数据 
	dat = 0;	                        	  //清除保存数据的变量
	for (i = 0; i < 8; i++){              	  //一个字节8位，循环8次	
   		dat <<= 1;                    		  //左移一位，腾出空位    
	    dat |= DHT11_Read_Bit();      		  //读取一位数据
    }						    
    return dat;                   			  //返回一个字节的数据
}

/*-------------------------------------------------*/
/*函数名：读取一次数据温湿度                       */
/*参  数：temp:温度值                              */
/*参  数：humi:湿度值                              */
/*返回值：1错误 0正确                              */
/*-------------------------------------------------*/ 
unsigned char dht11_read_data(unsigned char *temp, unsigned char *humi)    
{        
 	char buf[5];                              //一次完整的数据有5个字节，定义一个缓冲区
	
	char i;                                   //定义一个变量用于for循环  
	DHT11_Rst();                              //复位DHT11	
	// UsartPrintf(USART_DEBUG, "check DHT11(0--success):%d\r\n",DHT11_Check());
	if(DHT11_Check() == 0)				      //判断DHT11回复状态=0的话，表示正确，进入if
	{
		for(i = 0; i < 5; i++){               //一次完整的数据有5个字节，循环5次		
			buf[i] = DHT11_Read_Byte();       //每次读取一个字节
		}

		if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])//判断数据校验，前4个字节相加应该等于第5个字节，正确的话，进入if	
		{     
			// printf("%d\r\n",buf[0]); 	//humi数据
			// printf("%d\r\n",buf[1]);
			// printf("%d\r\n",buf[2]);		//temp数据
			// printf("%d\r\n",buf[3]);
			// printf("%d\r\n",buf[4]);
			*humi = buf[0];                   //湿度数据，保存在humi指针指向的地址变量中
			*temp = buf[2];					  //温度数据，保存在temp指针指向的地址变量中
		}else return 1;                       //反之，数据校验错误，直接返回1
	}else return 2;                           //反之，如果DHT11回复状态=1的话，表示错误，进入else，直接返回2
	
	return 0;	                              //读取正确返回0    
}  



// void led_on(void)
// {
//    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_85, RESET);
// 	 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_84, SET);
// }

// void led_off(void)
// {
//    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_85, SET);
// 	 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_84, RESET);
	
// }

void test(void)
{
	if((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET))
	{
		UsartPrintf(USART_DEBUG, "low\r\n");
	}
	if((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET))
	{
		UsartPrintf(USART_DEBUG, "high\r\n");
	}//调试用，勿理
}





