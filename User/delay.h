#ifndef __DELAY_H
#define __DELAY_H

#include "ht32.h"
// #include "ht32_board.h"
void delay_init(void);
void delay_ms(u32 nTime);
void delay_us(u32 nTime);
void delay_35us(u32 nTime);

#endif
