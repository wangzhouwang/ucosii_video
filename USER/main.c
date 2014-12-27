/**
  ******************************************************************************
  * @file    GPIO/IOToggle/main.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "ucos_ii.h"
#include <stdio.h>
#include "Gpio.h"
#include "uart.h"


#define STARTUP_TASK_PRIO     4
#define STARTUP_TASK_STK_SIZE 80

#define TASK1_PRIO     5
#define TASK1_STK_SIZE 128
#define TASK2_PRIO     6
#define TASK2_STK_SIZE 128

static void task1(void *p_arg);
static void task2(void *p_arg);

static OS_STK starup_task_stk[STARTUP_TASK_STK_SIZE];

static OS_STK task1_stk[TASK1_STK_SIZE];
static OS_STK task2_stk[TASK2_STK_SIZE];
OS_EVENT*	Sem1;

static void systick_init(void)
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    SysTick_Config(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);
}

static void startup_task(void *p_arg)
{
    systick_init();
#if(OS_TASK_STAT_EN > 0)
    OSStatInit();
#endif

    uart_init();
		printf("uart init finish\n");
#if	OS_SEM_EN>0
	/*创建一个拥有1个资源的信号量*/
	Sem1 = OSSemCreate(1);
	if(Sem1!=0){
		//printf("Error->Sem1 = OSSemCreate(3);\r\n");
	}
#endif

    
    OSTaskCreate(task1,(void*)0,&task1_stk[TASK1_STK_SIZE - 1],TASK1_PRIO);
    OSTaskCreate(task2,(void*)0,&task2_stk[TASK2_STK_SIZE - 1],TASK2_PRIO);

    OSTaskDel(OS_PRIO_SELF);
}


int main(void)
{
    OSInit();
    OSTaskCreate(startup_task,(void*)0,&starup_task_stk[STARTUP_TASK_STK_SIZE - 1],STARTUP_TASK_PRIO);
    OSStart();
    return 0;
}

static void task1(void *p_arg)
{
	INT8U	err;

    LEDGpio_Init();



    for(;;)
    {
      	/*请求信号量*/
		OSSemPend(Sem1,0,&err);
        printf("\r\n hello! \r\n");
        LED1_ONOFF(Bit_RESET);
        OSTimeDly(500);
        LED1_ONOFF(Bit_SET);
        OSTimeDly(500);
        LED2_ONOFF(Bit_RESET);
        OSTimeDly(500);
        LED2_ONOFF(Bit_SET);
        OSTimeDly(500);
        LED3_ONOFF(Bit_RESET);
        OSTimeDly(500);
        LED3_ONOFF(Bit_SET);
        OSTimeDly(500);

        /*释放信号量*/
		OSSemPost(Sem1);
    }
}

static void task2(void *p_arg)
{
	INT8U	err;

    for(;;)
    {   
        /*请求信号量*/
		OSSemPend(Sem1,0,&err);

        printf("\r\n hello! \r\n");
        LED4_ONOFF(Bit_RESET);
        OSTimeDly(500);
        LED4_ONOFF(Bit_SET);
        OSTimeDly(500);
        //
           
        /*释放信号量*/
		OSSemPost(Sem1); 
        OSTimeDly(500);        
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
