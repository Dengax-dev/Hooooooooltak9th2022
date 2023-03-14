/*********************************************************************************************************//**
 * @file    pwm.c
 * @version $Rev:: 5254         $
 * @date    $Date:: 2021-02-04 #$
 * @brief   The PWM functions.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board_config.h"
#include "pwm.h"
#include "ht32f5xxxx_01.h"


/* Private constants ---------------------------------------------------------------------------------------*/
#define HTCFG_PWM_TM_PORT                         STRCAT2(HT_,       HTCFG_PWM_TM_IPN)
#define HTCFG_PWM_TM_AFIO_FUN                     STRCAT2(AFIO_FUN_, HTCFG_PWM_TM_IPN)
#define HTCFG_PWM_TM_IRQn                         STRCAT2(HTCFG_PWM_TM_IPN,  _IRQn)
#define HTCFG_PWM_TM_IRQHandler                   STRCAT2(HTCFG_PWM_TM_IPN,  _IRQHandler)

#if (HTCFG_PWM_IDLE_STATE == 0)
#define HTCFG_PWM_IDLE_DUTY PWM_DUTY_0
#else
#define HTCFG_PWM_IDLE_DUTY PWM_DUTY_100
#endif

#if (HTCFG_PWM0_EN == 1)
#define HTCFG_PWM0_GPIO_ID                        STRCAT2(GPIO_P,    HTCFG_PWM0_GPIOX)
#define HTCFG_PWM0_AFIO_PIN                       STRCAT2(AFIO_PIN_, HTCFG_PWM0_GPION)
#define PWM_CH0                                   STRCAT2(TM_CH_,    HTCFG_PWM0_CHN)
#endif

#if (HTCFG_PWM1_EN == 1)
#define HTCFG_PWM1_GPIO_ID                        STRCAT2(GPIO_P,    HTCFG_PWM1_GPIOX)
#define HTCFG_PWM1_AFIO_PIN                       STRCAT2(AFIO_PIN_, HTCFG_PWM1_GPION)
#define PWM_CH1                                   STRCAT2(TM_CH_,    HTCFG_PWM1_CHN)
#endif

#if (HTCFG_PWM2_EN == 1)
#define HTCFG_PWM2_GPIO_ID                        STRCAT2(GPIO_P,    HTCFG_PWM2_GPIOX)
#define HTCFG_PWM2_AFIO_PIN                       STRCAT2(AFIO_PIN_, HTCFG_PWM2_GPION)
#define PWM_CH2                                   STRCAT2(TM_CH_,    HTCFG_PWM2_CHN)
#endif

#if (HTCFG_PWM3_EN == 1)
#define HTCFG_PWM3_GPIO_ID                        STRCAT2(GPIO_P,    HTCFG_PWM3_GPIOX)
#define HTCFG_PWM3_AFIO_PIN                       STRCAT2(AFIO_PIN_, HTCFG_PWM3_GPION)
#define PWM_CH3                                   STRCAT2(TM_CH_,    HTCFG_PWM3_CHN)
#endif

/* Private macro -------------------------------------------------------------------------------------------*/
#define IPN_NULL                        (0)
#define IPN_MCTM0                       (0x4002C000)
#define IPN_MCTM1                       (0x4002D000)
#define IPN_CHECK(IP)                   STRCAT2(IPN_, IP)
#define IS_IPN_MCTM(IP)                 (IPN_CHECK(IP) == IPN_MCTM0) || (IPN_CHECK(IP) == IPN_MCTM1)

/* Private variables ---------------------------------------------------------------------------------------*/
s32 gIsTMStop = 0;

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Init PWM function.
  * @retval None
  ***********************************************************************************************************/
void PWM_Init(void)
{
  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
    CKCUClock.Bit.HTCFG_PWM_TM_IPN = 1;
    CKCUClock.Bit.AFIO             = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Configure AFIO mode as TM function                                                                     */
  #if (HTCFG_PWM0_EN == 1)
  AFIO_GPxConfig(HTCFG_PWM0_GPIO_ID, HTCFG_PWM0_AFIO_PIN, HTCFG_PWM_TM_AFIO_FUN);
  #endif
  #if (HTCFG_PWM1_EN == 1)
  AFIO_GPxConfig(HTCFG_PWM1_GPIO_ID, HTCFG_PWM1_AFIO_PIN, HTCFG_PWM_TM_AFIO_FUN);
  #endif
  #if (HTCFG_PWM2_EN == 1)
  AFIO_GPxConfig(HTCFG_PWM2_GPIO_ID, HTCFG_PWM2_AFIO_PIN, HTCFG_PWM_TM_AFIO_FUN);
  #endif
  #if (HTCFG_PWM3_EN == 1)
  AFIO_GPxConfig(HTCFG_PWM3_GPIO_ID, HTCFG_PWM3_AFIO_PIN, HTCFG_PWM_TM_AFIO_FUN);
  #endif


  { /* Time base configuration                                                                              */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below this function.
    */
    TM_TimeBaseInitTypeDef TimeBaseInit;

    TimeBaseInit.Prescaler = HTCFG_PWM_TM_PRESCALER - 1;            // Timer clock = CK_AHB / HTCFG_PWM_TM_PRESCALER 预分频
    TimeBaseInit.CounterReload = HTCFG_PWM_TM_RELOAD - 1; //重装载值，周期
    TimeBaseInit.RepetitionCounter = 0;
    TimeBaseInit.CounterMode = TM_CNT_MODE_UP;
    TimeBaseInit.PSCReloadTime = TM_PSC_RLD_IMMEDIATE;
    TM_TimeBaseInit(HTCFG_PWM_TM_PORT, &TimeBaseInit);
    TM_CRRPreloadCmd(HTCFG_PWM_TM_PORT, ENABLE);

    /* Clear Update Event Interrupt flag since the "TM_TimeBaseInit()" writes the UEV1G bit                 */
    TM_ClearFlag(HT_GPTM0, TM_FLAG_UEV);
  }

  { /* Channel n output configuration                                                                       */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below this function.
    */
    TM_OutputInitTypeDef OutInit;

    OutInit.OutputMode = TM_OM_PWM1;
    OutInit.Control = TM_CHCTL_ENABLE;
    OutInit.ControlN = TM_CHCTL_DISABLE;
    OutInit.Polarity = TM_CHP_NONINVERTED;
    OutInit.PolarityN = TM_CHP_NONINVERTED;
    OutInit.IdleState = MCTM_OIS_LOW;
    OutInit.IdleStateN = MCTM_OIS_HIGH;
    OutInit.Compare = HTCFG_PWM_IDLE_DUTY; //此处决定占空比
    OutInit.AsymmetricCompare = 0;

    #if (HTCFG_PWM0_EN == 1)
    OutInit.Channel = PWM_CH0;
    TM_OutputInit(HTCFG_PWM_TM_PORT, &OutInit); // CH0 Start Output as default value
    TM_CHCCRPreloadConfig(HTCFG_PWM_TM_PORT, PWM_CH0, ENABLE);
    #endif

    #if (HTCFG_PWM1_EN == 1)
    OutInit.Channel = PWM_CH1;
    TM_OutputInit(HTCFG_PWM_TM_PORT, &OutInit); // CH1 Start Output as default value
    TM_CHCCRPreloadConfig(HTCFG_PWM_TM_PORT, PWM_CH1, ENABLE);
    #endif

    #if (HTCFG_PWM2_EN == 1)
    OutInit.Channel = PWM_CH2;
    TM_OutputInit(HTCFG_PWM_TM_PORT, &OutInit); // CH2 Start Output as default value
    TM_CHCCRPreloadConfig(HTCFG_PWM_TM_PORT, PWM_CH2, ENABLE);
    #endif

    #if (HTCFG_PWM3_EN == 1)
    OutInit.Channel = PWM_CH3;
    TM_OutputInit(HTCFG_PWM_TM_PORT, &OutInit); // CH3 Start Output as default value
    TM_CHCCRPreloadConfig(HTCFG_PWM_TM_PORT, PWM_CH3, ENABLE);
    #endif
  }

  #if (IS_IPN_MCTM(HTCFG_PWM_TM_IPN))
  {
    // Only MCTM need OE Enable
    MCTM_CHMOECmd(HTCFG_PWM_TM_PORT, ENABLE);

    // Enable MT_CHnN
    //MCTM_ChannelNConfig(HTCFG_PWM_TM_PORT, HTCFG_PWM_TM_CH, TM_CHCTL_ENABLE);
  }
  #endif

  /* Enable Update Event interrupt                                                                          */
  NVIC_EnableIRQ(HTCFG_PWM_TM_IRQn);

  #if 0 // Default enable or disable
  TM_Cmd(HTCFG_PWM_TM_PORT, ENABLE);
  #endif
}

/*********************************************************************************************************//**
  * @brief  This function handles TM interrupt.
  * @retval None
  ***********************************************************************************************************/
void HTCFG_PWM_TM_IRQHandler(void)
{
  TM_ClearFlag(HTCFG_PWM_TM_PORT, TM_INT_UEV);

  if (gIsTMStop == 1)
  {
    gIsTMStop = 2;
  }
  else if (gIsTMStop == 2)
  {
    TM_Cmd(HTCFG_PWM_TM_PORT, DISABLE);

    TM_IntConfig(HTCFG_PWM_TM_PORT, TM_INT_UEV, DISABLE);

    gIsTMStop = 0;
  }
}

/*********************************************************************************************************//**
  * @brief Enable or Disable PWM.
  * @param NewState: This parameter can be ENABLE or DISABLE.
  * @retval None
  ***********************************************************************************************************/
void PWM_Cmd(ControlStatus NewState)
{
  if (NewState == ENABLE)
  {
    TM_Cmd(HTCFG_PWM_TM_PORT, NewState);
  }
  else
  {
    // Set the flag to stop timer after update event.
    gIsTMStop = 1;

    // Set PWM as IDLE duty before stop it
    #if (HTCFG_PWM0_EN == 1)
    PWM_UpdateDuty(PWM_CH0, HTCFG_PWM_IDLE_DUTY);
    #endif
    #if (HTCFG_PWM1_EN == 1)
    PWM_UpdateDuty(PWM_CH1, HTCFG_PWM_IDLE_DUTY);
    #endif
    #if (HTCFG_PWM2_EN == 1)
    PWM_UpdateDuty(PWM_CH2, HTCFG_PWM_IDLE_DUTY);
    #endif
    #if (HTCFG_PWM3_EN == 1)
    PWM_UpdateDuty(PWM_CH3, HTCFG_PWM_IDLE_DUTY);
    #endif

    TM_IntConfig(HTCFG_PWM_TM_PORT, TM_INT_UEV, ENABLE);
  }
}

#if 0
/*********************************************************************************************************//**
  * @brief  Set PWM Frequency
  * @param  uReload: Reload value of timer)
  * @retval None
  ***********************************************************************************************************/
void PWM_SetFreq(u32 uReload)
{
  TM_SetCounterReload(HTCFG_PWM_TM_PORT, uReload);
}

/*********************************************************************************************************//**
  * @brief  Update PWM Duty
  * @param  TM_CH_n: Specify the TM channel.
  * @param  uCompare: PWM duty (Compare value of timer)
  * @retval None
  ***********************************************************************************************************/
void PWM_UpdateDuty(TM_CH_Enum TM_CH_n, u32 uCompare)
{
  TM_SetCaptureCompare(HTCFG_PWM_TM_PORT, TM_CH_n, uCompare);
}
#endif
