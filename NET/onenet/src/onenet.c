//单片机头文件
#include "HT32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "led.h"
#include "pwm.h"
//C库
#include <string.h>
#include <stdio.h>

#define PROID		"492294"   //产品ID

#define AUTH_INFO	"111222333"		//鉴权信息

#define DEVID		"902794077"	//设备ID

extern unsigned char esp8266_buf[128];

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern unsigned char temper ,humidity;
extern int automatic; //自动化控制
extern int rain; //雨水状态标志位
extern int curtainWifi; //防雨帘标志位
extern int light; //灯状态标志位
extern int light_con;
extern int fan; //风扇状态标志位
extern int hungerWifi; //晾衣架
extern unsigned char key_velue = 0; //开关值

unsigned char OneNet_FillBuf(char *buf)
{
	char text[32];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "key,%d;", key_velue);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Humi,%d;", humidity);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Temp,%d;", temper);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Rain,%d;", rain);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Curtain,%d;", curtainWifi);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Light,%d;", light);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Fan,%d;", fan);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Auto,%d;", automatic);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Hunger,%d;", hungerWifi);
	strcat(buf, text);

	return strlen(buf);
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	printf("OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK: //发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//搜索':'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		// UsartPrintf(USART_DEBUG, "test:-----------------找到了\r\n");
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);			//转为数值形式
		UsartPrintf(USART_DEBUG, "num=%d\r\n",num);
		
		if(strstr((char *)req_payload, "key"))		//搜索"key"
		{
			key_velue = num;                		 //更新数据
			UsartPrintf(USART_DEBUG, "key=%d\r\n",key_velue);
		}

		else if(strstr((char *)req_payload, "fan"))	//搜索"fan"
		{
			fan = num;
			if(fan == 1) PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_100); //开启风扇
			else PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //关闭风扇
			UsartPrintf(USART_DEBUG, "fan=%d\r\n",fan);
		}

		else if(strstr((char *)req_payload, "cur"))	//搜索"curtain"
		{
			curtainWifi = num;
			UsartPrintf(USART_DEBUG, "curtain=%d\r\n",curtainWifi);
		}

		else if(strstr((char *)req_payload, "lig"))	//搜索"light"
		{
			light = num;
			light_con = num;
			if(light == 1) GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, SET); //控制开关灯
			else GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, RESET);
			UsartPrintf(USART_DEBUG, "light=%d\r\n",light);
		}
		else if(strstr((char *)req_payload, "aut"))	//搜索"automatic"
		{
			automatic = num;
			UsartPrintf(USART_DEBUG, "automatic=%d\r\n",automatic);
		}
		else if(strstr((char *)req_payload, "hun"))	//搜索"automatic"
		{
			hungerWifi = num;
			UsartPrintf(USART_DEBUG, "hungerWifi=%d\r\n",hungerWifi);
		}

		else UsartPrintf(USART_DEBUG, "未找到命令\r\n");
	} 

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
