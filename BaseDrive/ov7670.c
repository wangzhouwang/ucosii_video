/**
  ******************************************************************************
  * @file    ov7670.c 
  * @author  Wangbao Dev Team
  * @version V1.0.0
  * @date    07-10-2012
  * @brief   uart program body
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
#include "ov7670.h"
#include "sccb_bus.h"
#include "lcd.h"
#include "ov7670_table.h"

#define LCD_BUF_SIZE    16
//uint32_t LCD_BUF[32000]={Blue};
 uint32_t LCD_BUF[LCD_BUF_SIZE]={Blue};

static void _delay(uint32_t dl)
{
	unsigned int i,y;
	for(i = 0; i < 1000; i++)
	{
		for(y = 0; y < dl; y++);
	}
}

static void port_dma_init(void)
{
	DCMI_InitTypeDef DCMI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);//DCMI 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
		RCC_AHB1Periph_GPIOC, ENABLE);//使能DCMI的GPIO时钟
	

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);//DCMI_HSYNC 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_PIXCLK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_D5 			  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);//DCMI_VSYNC 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_DCMI);//DCMI_D6 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_DCMI);//DCMI_D7 			  
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_D0 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);//DCMI_D1 			  
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);//DCMI_D2 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);//DCMI_D3 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);//DCMI_D4 

	/* DCMI GPIO configuration ************************************************* */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
	DCMI_Init(&DCMI_InitStructure);

	/* DCMI Interrupts config ***************************************************/
	DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);	

	
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 


	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)LCD_BUF;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = LCD_BUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

void dma_out_lcd_config(void)		   	
{

	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  

	DMA_DeInit(DMA2_Stream4);

	DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)LCD_BUF;	
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&LCD_RAM;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
	DMA_InitStructure.DMA_BufferSize = LCD_BUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream4, &DMA_InitStructure); 
	DMA_ITConfig(DMA2_Stream4, DMA_IT_TC, ENABLE);


	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  

}






//写OV7670寄存器
uint8_t ov7670_write_reg(uint8_t reg, uint8_t data)
{
	sccb_bus_start();//发送SCCB 总线开始传输命令
	if(sccb_bus_write_byte(0x42) == 0)//写地址
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 1;//错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg) == 0)//ID
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 2;//错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(data)==0)//写数据到积存器
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 3;//错误返回
	}
	sccb_bus_stop();//发送SCCB 总线停止传输命令	
	return 0;//成功返回
}
//读OV7660寄存器
uint8_t ov7660_read_reg(uint8_t reg, uint8_t *ret_reg)
{
	//通过写操作设置寄存器地址
	sccb_bus_start();
	if(sccb_bus_write_byte(0x42) == 0)//写地址
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 1;//错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg)==0)//ID
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 2;//错误返回
	}
	sccb_bus_stop();//发送SCCB 总线停止传输命令	
	systick_delay_us(10);	
	//设置寄存器地址后，才是读
	sccb_bus_start();
	if(sccb_bus_write_byte(0x43)==0)//读地址
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		return 3;//错误返回
	}
	systick_delay_us(10);
	*ret_reg = sccb_bus_read_byte();//返回读到的值
	sccb_bus_send_noack();//发送NACK命令
	sccb_bus_stop();//发送SCCB 总线停止传输命令
	return 0;//成功返回
}

void ov7670_reset(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOF,GPIO_Pin_10);
	_delay(500);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
	_delay(500);

	ov7670_write_reg(0x12,0x80);
}
uint8_t ov7660_get_id(void)
{
	uint8_t temp;
	ov7660_read_reg(0x0b,&temp);
	return temp;
}

uint8_t ov7670_check(void)
{
	uint8_t ovid;
	dma_out_lcd_config();
	port_dma_init();
	sccb_bus_init();
	ov7670_reset();
	systick_delay_ms(5);
	ovid = ov7660_get_id();
	
	return ovid; 
}

void cam_start(void)
{	
	DMA_Cmd(DMA2_Stream1, ENABLE); 
	DCMI_Cmd(ENABLE); 
	DCMI_CaptureCmd(ENABLE); 
	//rw_data_prepare();
	LCD_REG = 0x22;
}

uint8_t ov7670_init(void)
{
	uint32_t i;

	
	
	for(i = 0; i < OV7670_REG_NUM; i++)
	{
		if(ov7670_write_reg(OV7670_reg[i][0],OV7670_reg[i][1]))
		{
			return 1;
		}
	}
	return 0;	
}



/***************************************************************************************
*下面是中断程序
*/

void DCMI_IRQHandler(void)
{  	 
	OSIntEnter();
	
	if (DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) 
	{	 		   	
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
		//GPIO_ToggleBits(GPIOE, GPIO_Pin_1);	
	}

	OSIntExit();
}

void DMA2_Stream4_IRQHandler(void) 	//LCD??ê?íê±?
{
	OSIntEnter();
	
	/* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA2_Stream4, DMA_IT_TCIF4))
	{
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream4, DMA_IT_TCIF4);
	}

	OSIntExit();
}

void DMA2_Stream1_IRQHandler(void) 	 //é???í·2é?ˉíê±?
{
    int i;

	OSIntEnter();
	/* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
	{	
		//GPIO_ToggleBits(GPIOE, GPIO_Pin_2);
		/* Clear DMA Stream Transfer Complete interrupt pending bit */
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);

//        lcd_SetCursor(0,0);
//        rw_data_prepare();
//        
//        
//        for(i=0;i<100000;i++);

		DMA_Cmd(DMA2_Stream4, ENABLE);
	}

	OSIntExit();
}

