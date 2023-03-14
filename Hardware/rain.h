#ifndef __RAIN_H_
#define __RAIN_H_

void Rain_Init(void);
int Rain(void);

void motorInit(void); //电机辅助初始化

void cloth_Init(void);
int Cloth(void);

void curtainDown(void); //放下帘子
void curtainUp(void); //升起帘子

void hungerUp(void); //升起晾衣架
void hungerDown(void); //放下晾衣架

#endif

