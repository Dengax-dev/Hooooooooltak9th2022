//��Ƭ��ͷ�ļ�
#include "HT32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "led.h"
#include "pwm.h"
//C��
#include <string.h>
#include <stdio.h>

#define PROID		"492294"   //��ƷID

#define AUTH_INFO	"111222333"		//��Ȩ��Ϣ

#define DEVID		"902794077"	//�豸ID

extern unsigned char esp8266_buf[128];

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
extern unsigned char temper ,humidity;
extern int automatic; //�Զ�������
extern int rain; //��ˮ״̬��־λ
extern int curtainWifi; //��������־λ
extern int light; //��״̬��־λ
extern int light_con;
extern int fan; //����״̬��־λ
extern int hungerWifi; //���¼�
extern unsigned char key_velue = 0; //����ֵ

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
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	printf("OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
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
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK: //����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//����':'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		// UsartPrintf(USART_DEBUG, "test:-----------------�ҵ���\r\n");
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);			//תΪ��ֵ��ʽ
		UsartPrintf(USART_DEBUG, "num=%d\r\n",num);
		
		if(strstr((char *)req_payload, "key"))		//����"key"
		{
			key_velue = num;                		 //��������
			UsartPrintf(USART_DEBUG, "key=%d\r\n",key_velue);
		}

		else if(strstr((char *)req_payload, "fan"))	//����"fan"
		{
			fan = num;
			if(fan == 1) PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_100); //��������
			else PWM_UpdateDuty(STRCAT2(TM_CH_,    2), PWM_DUTY_0); //�رշ���
			UsartPrintf(USART_DEBUG, "fan=%d\r\n",fan);
		}

		else if(strstr((char *)req_payload, "cur"))	//����"curtain"
		{
			curtainWifi = num;
			UsartPrintf(USART_DEBUG, "curtain=%d\r\n",curtainWifi);
		}

		else if(strstr((char *)req_payload, "lig"))	//����"light"
		{
			light = num;
			light_con = num;
			if(light == 1) GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, SET); //���ƿ��ص�
			else GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_7, RESET);
			UsartPrintf(USART_DEBUG, "light=%d\r\n",light);
		}
		else if(strstr((char *)req_payload, "aut"))	//����"automatic"
		{
			automatic = num;
			UsartPrintf(USART_DEBUG, "automatic=%d\r\n",automatic);
		}
		else if(strstr((char *)req_payload, "hun"))	//����"automatic"
		{
			hungerWifi = num;
			UsartPrintf(USART_DEBUG, "hungerWifi=%d\r\n",hungerWifi);
		}

		else UsartPrintf(USART_DEBUG, "δ�ҵ�����\r\n");
	} 

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
