/**
  ******************************************************************************
  * @file    k_bsp.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel bsp functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "k_bsp.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_BSP
  * @brief Kernel BSP routines
  * @{
  */

/* External variables --------------------------------------------------------*/
extern uint8_t  I2C_Address;
TS_StateTypeDef  TS_State = {0};
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes LEDs, SDRAM, touch screen, CRC and SRAM.
  * @param  None 
  * @retval None
  */
void k_BspInit(void)
{
	/* Initialize the RTC */
	k_CalendarBkupInit();

	/* Initialize the LEDS */
	BSP_LED_Init(LED_GREEN);
	BSP_LED_Init(LED_ORANGE);
	BSP_LED_Init(LED_BLUE);
	BSP_LED_Init(LED_RED);

	/* Initialize the NOR */
	BSP_QSPI_Init();
	BSP_QSPI_EnableMemoryMappedMode();


	/* Initialize the SDRAM */
	//  BSP_SDRAM_Init(); ->Done in BSP_LCD_Init()

	HAL_Delay(100);

	/* Enable Back up SRAM */
	__HAL_RCC_BKPSRAM_CLK_ENABLE();
}

/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
uint8_t k_TouchUpdate(void)
{
  __IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;
  uint8_t result = 0;

  BSP_TS_GetState((TS_StateTypeDef *)&ts);

  if((ts.touchX[0] >= BSP_LCD_GetXSize()) ||(ts.touchY[0] >= BSP_LCD_GetYSize()) )
  {
    ts.touchX[0] = 0;
    ts.touchY[0] = 0;
  }

  xDiff = (TS_State.touchX[0] > ts.touchX[0]) ?
		  (TS_State.touchX[0] - ts.touchX[0]) : (ts.touchX[0] - TS_State.touchX[0]);
  yDiff = (TS_State.touchY[0] > ts.touchY[0]) ?
		  (TS_State.touchY[0] - ts.touchY[0]) : (ts.touchY[0] - TS_State.touchY[0]);

  if((TS_State.touchDetected != ts.touchDetected ) || (xDiff > 20 )|| (yDiff > 20))
  {
    TS_State.touchDetected = ts.touchDetected;
    result = 1;

    if(ts.touchDetected)
    {
      TS_State.touchX[0] = ts.touchX[0];

      if(I2C_Address == TS_I2C_ADDRESS)
      {
        if(ts.touchY[0] < 240)
        {
          TS_State.touchY[0] = ts.touchY[0];
        }
        else
        {
          TS_State.touchY[0] = (ts.touchY[0] * 480) / 450;
        }
      }
      else
      {
        TS_State.touchY[0] = ts.touchY[0];
      }

    }
    else
    {
      TS_State.touchX[0] = 0;
      TS_State.touchY[0] = 0;
    }
  }
  return result;
}


TS_StateTypeDef* k_GetTouchCoordinates(void){
	return &TS_State;
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
