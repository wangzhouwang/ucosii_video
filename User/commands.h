/*	  
 *	commands.h 
 *	the genie shell commands part for ucosII 
 *	under skyeye 
 * 
 *	Bugs report:	 Yang Ye  ( yangye@163.net ) 
 *	Last modified:	 2003-02-19  
 * 
 */ 
#ifndef __COMMANDS_H__
#define __COMMANDS_H__
#include "ucos_ii.h"
#include "stdio.h"
#include "string.h"
 
#define MAX_COMMAND_NUM    16
 
typedef struct{ 
	int 			num; 			//命令的当前数量
	char 		*name; 		//命令名称
	char			*help;			//命令帮助信息
	INT8U (*CommandFunc)(INT8U argc,char **argv); 
}command; 

 
/* shell 模块初始化 */
void shell_task_init(void);

/* 命令导出 */
void command_export(INT8U (*CommandFunc)(INT8U argc,char **argv),
											const char *Name,
											const char *help);

#endif
