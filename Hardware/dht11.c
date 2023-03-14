#include "ht32.h"
#include "ht32_board.h"
#include "dht11.h"
#include "delay.h"
#include "usart.h"

	// USART1_Configuration();				//����1����ӡ��Ϣ��


void dht11_init(void)
{
	CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
	LEDClock.Bit.PB = 1; //ʹ��PC��//�ٽ�����Ҫ��ʱ���Է��ʳ�Ա��������ʽ������bit���������� ��3*32λ�Ľṹ�� �����³�Ա��Ϊ��Ӧ�����ʱ��λ����һ����ʱ�ӵ����üĴ���д��һ
	CKCU_PeripClockConfig(LEDClock, ENABLE); //һ��Ҫ��ʹ��ʱ�ӣ������������
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //����PC1Ϊ���

    DHT11_Rst();
	// DHT11_Check();
    UsartPrintf(USART_DEBUG, "check DHT11(0--success):%d\r\n",DHT11_Check());
	
}

void DHT11_Rst(void)	
{
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //����PC1Ϊ���
    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, RESET);
    delay_ms(30);     //�������Ϳ�ʼ�źţ����ͣ�����������18ms����������30
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_OUT); //����PC1Ϊ���
    GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, SET);  //PC1����DHT11
    delay_us(30);     //�������Ϳ�ʼ�źţ����ߣ�����������20~40us����������40us
    // GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_8, SET);
    
    // UsartPrintf(USART_DEBUG, "reset success\r\n");
}


/*-------------------------------------------------*/
/*���������ȴ�DHT11�Ļ�Ӧ                          */
/*��  ������                                       */
/*����ֵ��1���� 0��ȷ                              */
/*-------------------------------------------------*/ 
char DHT11_Check(void) 	   
{   
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //����PC1Ϊ����
	GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//�ڲ���������������,����
	GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//����ʹ��
	// delay_us(20);
	char timeout1,timeout2;                            //����һ���������ڳ�ʱ�ж�  
	timeout1 = 0;                             //��ʱ��������    
	timeout2 =0;
    while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout1 < 70))//DHT11������40~50us,���ǵȴ�70us��ʱʱ��	
	{	 
		timeout1++;                           //��ʱ����+1
		delay_us(1);                       	 //��ʱ1us
	} 

	if(timeout1 >= 70)return 1;               //���timeout>=100,˵������Ϊ��ʱ�˳���whileѭ��������1��ʾ����
	// else timeout = 0;                        //��֮��˵������Ϊ�ȵ���DHT11����IO���˳���whileѭ������ȷ������timeout
    while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout2 < 70))//DHT11���ͺ���ٴ�����40~50us,,���ǵȴ�70us��ʱʱ��	
	{ 		
		timeout2++;                           //��ʱ����+1
		delay_us(1);                          
	}
	// UsartPrintf(USART_DEBUG, "timeout1;%d\r\n",timeout1);
	// UsartPrintf(USART_DEBUG, "timeout2;%d\r\n",timeout2);

	if(timeout2 >= 70)return 2;               //���timeout>=70,˵������Ϊ��ʱ�˳���whileѭ��������2��ʾ����  
	return 0;                                //��֮��ȷ������0
}

/*-------------------------------------------------*/
/*����������ȡһ��λ                               */
/*��  ������                                       */
/*����ֵ��1��0                                     */
/*-------------------------------------------------*/ 

char DHT11_Read_Bit(void) 			 
{
	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //����PC1Ϊ����
	// GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//�ڲ���������������
	// GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//����ʹ��

 	char timeout;                          	   //����һ���������ڳ�ʱ�ж�  
	
	timeout = 0;                               //����timeout	
	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout < 40))//ÿһλ���ݿ�ʼ����12~14us�ĵ͵�ƽ�����ǵ�40us
	{   
		timeout++;                             //��ʱ����+1
		delay_us(1);                            
	}
	timeout = 0;                               //����timeout	
	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout < 60))//��������DHT11������IO���������ߵ�ʱ���ж���0��1�����ǵ�60us
	{  
		timeout++;                             //��ʱ����+1
		delay_us(1);                            
	}

	// delay_us(30); 
	delay_us(30);                              
	
	if(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET)return 1;                   //�����ʱ���Ǹߵ�ƽ����ô��λ���յ���1������1
	else return 0;		                       //��֮��ʱ���ǵ͵�ƽ����ô��λ���յ���0������0
	// delay_us(30); 
}

// char DHT11_Read_Bit(void) 			 
// {
// 	// GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_8, GPIO_DIR_IN); //����PC1Ϊ����
// 	// GPIO_PullResistorConfig(HT_GPIOB,GPIO_PIN_8,GPIO_PR_UP);//�ڲ���������������
// 	// GPIO_InputConfig(HT_GPIOB,GPIO_PIN_8,ENABLE);//����ʹ��

//  	char timeout;                          	   //����һ���������ڳ�ʱ�ж�  
	
// 	timeout = 0;                               //����timeout	
// 	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET) && (timeout < 40))//ÿһλ���ݿ�ʼ����12~14us�ĵ͵�ƽ�����ǵ�40us
// 	{   
// 		timeout++;                             //��ʱ����+1
// 		delay_us(1);                            
// 	}
// 	timeout = 0;                               //����timeout	
// 	while((GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == RESET) && (timeout < 60))//��������DHT11������IO���������ߵ�ʱ���ж���0��1�����ǵ�60us
// 	{  
// 		timeout++;                             //��ʱ����+1
// 		delay_us(1);                            
// 	}
// 	delay_us(35);                               
// 	if(GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_8) == SET)return 1;                   //�����ʱ���Ǹߵ�ƽ����ô��λ���յ���1������1
// 	else return 0;		                       //��֮��ʱ���ǵ͵�ƽ����ô��λ���յ���0������0
// 	// delay_us(30); 
// }

/*-------------------------------------------------*/
/*����������ȡһ���ֽ�                              */
/*��  ������                                       */
/*����ֵ������                                      */
/*-------------------------------------------------*/ 
char DHT11_Read_Byte(void)    
{        
    char i;                       			  //����һ����������forѭ��  
	char dat;                              	  //����һ���������ڱ������� 
	dat = 0;	                        	  //����������ݵı���
	for (i = 0; i < 8; i++){              	  //һ���ֽ�8λ��ѭ��8��	
   		dat <<= 1;                    		  //����һλ���ڳ���λ    
	    dat |= DHT11_Read_Bit();      		  //��ȡһλ����
    }						    
    return dat;                   			  //����һ���ֽڵ�����
}

/*-------------------------------------------------*/
/*����������ȡһ��������ʪ��                       */
/*��  ����temp:�¶�ֵ                              */
/*��  ����humi:ʪ��ֵ                              */
/*����ֵ��1���� 0��ȷ                              */
/*-------------------------------------------------*/ 
unsigned char dht11_read_data(unsigned char *temp, unsigned char *humi)    
{        
 	char buf[5];                              //һ��������������5���ֽڣ�����һ��������
	
	char i;                                   //����һ����������forѭ��  
	DHT11_Rst();                              //��λDHT11	
	// UsartPrintf(USART_DEBUG, "check DHT11(0--success):%d\r\n",DHT11_Check());
	if(DHT11_Check() == 0)				      //�ж�DHT11�ظ�״̬=0�Ļ�����ʾ��ȷ������if
	{
		for(i = 0; i < 5; i++){               //һ��������������5���ֽڣ�ѭ��5��		
			buf[i] = DHT11_Read_Byte();       //ÿ�ζ�ȡһ���ֽ�
		}

		if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])//�ж�����У�飬ǰ4���ֽ����Ӧ�õ��ڵ�5���ֽڣ���ȷ�Ļ�������if	
		{     
			// printf("%d\r\n",buf[0]); 	//humi����
			// printf("%d\r\n",buf[1]);
			// printf("%d\r\n",buf[2]);		//temp����
			// printf("%d\r\n",buf[3]);
			// printf("%d\r\n",buf[4]);
			*humi = buf[0];                   //ʪ�����ݣ�������humiָ��ָ��ĵ�ַ������
			*temp = buf[2];					  //�¶����ݣ�������tempָ��ָ��ĵ�ַ������
		}else return 1;                       //��֮������У�����ֱ�ӷ���1
	}else return 2;                           //��֮�����DHT11�ظ�״̬=1�Ļ�����ʾ���󣬽���else��ֱ�ӷ���2
	
	return 0;	                              //��ȡ��ȷ����0    
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
	}//�����ã�����
}





