#include "delay.h"
#include "usart.h"



/* Private variables ---------------------------------------------------------------------------------------*/
static vu32 DelayTime;
/* Private function prototypes -----------------------------------------------------------------------------*/
void Delay(u32 nTime);
void TimingDelay(void);

void delay_init()
{
	/* SYSTICK configuration */
  SYSTICK_ClockSourceConfig(SYSTICK_SRC_STCLK);       // 即默认选择了外部参考时钟HSI(8MHz/8)
  SYSTICK_IntConfig(DISABLE);                          // Enable SYSTICK Interrupt

}

void delay_ms(u32 ms)
{
  
	u32 i;
  SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000); // (CK_SYS/8/1000) = 1ms 
 
	/* 打开SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	
	for( i = 0;i < ms;i++ )
	{
		while( !( (SysTick->CTRL) & (1<<16) ) ); 
	}
 
	/* 关闭SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
	/* 复位SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);

}


void delay_us(u32 us)
{

	u32 i;
	
	/* SYSTICK configuration */
	SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000000); // (CK_SYS/8/1000000) = 1us 
 
	/* 打开SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
 
	for( i = 0;i < us;i++ )
	{
		while( !( (SysTick->CTRL) & (1<<16) ) ); 
	}
 
	/* 关闭SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
	/* 复位SysTick计数器 */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);

}

// void delay_35us(u32 nTime)
// {

// 	SYSTICK_SetReloadValue(SystemCoreClock*35 / 8 / 1000000); // (CK_AHB/8/1000) = 1us on chip

//   /* Enable the SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	
	
	
//   DelayTime = nTime;

//   while(DelayTime != 0);

//   /* Disable SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
//   /* Clear SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
// }

/*********************************************************************************************************//**
  * @brief  Decrements the TimingDelay variable.
  * @retval None
  ***********************************************************************************************************/
void TimingDelay(void)
{
  if(DelayTime != 0)
  {
    DelayTime--;
  }
}



// /* Private variables ---------------------------------------------------------------------------------------*/
// static vu32 DelayTime;
// /* Private function prototypes -----------------------------------------------------------------------------*/
// void Delay(u32 nTime);
// void TimingDelay(void);

// void delay_init()
// {
// 	/* SYSTICK configuration */
//   SYSTICK_ClockSourceConfig(SYSTICK_SRC_STCLK);       // Default : CK_AHB/8
//   SYSTICK_IntConfig(ENABLE);                          // Enable SYSTICK Interrupt

// }



// void delay_ms(u32 nTime)
// {
//   SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000); // (CK_AHB/8/1000) = 1ms on chip

//   /* Enable the SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	
	
	
//   DelayTime = nTime;

//   while(DelayTime != 0);

//   /* Disable SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
//   /* Clear SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
// }


// void delay_us(u32 nTime)
// {

// 	SYSTICK_SetReloadValue(SystemCoreClock / 8 / 1000000); // (CK_AHB/8/1000) = 1us on chip

//   /* Enable the SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	
	
	
//   DelayTime = nTime;

//   while(DelayTime != 0);

//   /* Disable SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
//   /* Clear SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
// }

// void delay_35us(u32 nTime)
// {

// 	SYSTICK_SetReloadValue(SystemCoreClock*35 / 8 / 1000000); // (CK_AHB/8/1000) = 1us on chip

//   /* Enable the SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);
	
	
	
//   DelayTime = nTime;

//   while(DelayTime != 0);

//   /* Disable SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
//   /* Clear SysTick Counter */
//   SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
// }

// /*********************************************************************************************************//**
//   * @brief  Decrements the TimingDelay variable.
//   * @retval None
//   ***********************************************************************************************************/
// void TimingDelay(void)
// {
//   if(DelayTime != 0)
//   {
//     DelayTime--;
//   }
// }

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif

