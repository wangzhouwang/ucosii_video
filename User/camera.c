#include "appcommon.h"

#ifndef CAMERA_TASK_STK_SIZE
#define CAMERA_TASK_STK_SIZE      128
#endif

#ifndef CAMERA_TASK_PRIO
#define CAMERA_TASK_PRIO					8
#endif

#define  OS_USING_DEBUG
#ifdef OS_USING_DEBUG
#define kprintf		printf
#else
#define kprintf(...)		
#endif


static OS_STK task_stk[CAMERA_TASK_STK_SIZE];



static void camera_task_entry(void *p_arg)
{
	kprintf("%d camera task entry\n\r");
	ov7670_check();
	ov7670_init();
	lcd_init();
	OSTimeDly(500);
	cam_start();
	while(1)
	{
		OSTimeDly(1);
	}
}
void camera_task_init(void)
{
	INT8U result;
	
	result = OSTaskCreate(camera_task_entry,(void*)0,&task_stk[CAMERA_TASK_STK_SIZE - 1],CAMERA_TASK_PRIO);
	if(result != OS_ERR_NONE)
	{
		kprintf("Camera Task Create Fail\n\r");
	}
}

