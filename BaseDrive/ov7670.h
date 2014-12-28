/**
  ******************************************************************************
  * @file    ov7670.h 
  * @author  WangBao Dev Team
  * @version V1.0.0
  * @date    07-10-2012
  * @brief   Header for ov7670.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
 
#ifndef _OV7670_H_
#define _OV7670_H_


#include "stm32f4xx.h"

#define DCMI_DR_ADDRESS       	0x50050028   
#define FSMC_LCD_ADDRESS      	0x6C000002


uint8_t ov7670_check(void);
uint8_t ov7670_init(void);
void cam_start(void);


#endif /* _OV7670_H_ */

/******************* (C) COPYRIGHT 2012 WangBao *****END OF FILE****/
