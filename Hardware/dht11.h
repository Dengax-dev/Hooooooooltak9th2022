#ifndef __DHT11_H_
#define __DHT11_H_

#include "ht32f5xxxx_ckcu.h"
#include "ht32f5xxxx_gpio.h"


void dht11_init(void);
void DHT11_Rst(void);
char DHT11_Check(void);
char DHT11_Read_Bit(void);
char DHT11_Read_Byte(void);
unsigned char dht11_read_data(unsigned char *temp, unsigned char *humi);
void test(void);


#endif



