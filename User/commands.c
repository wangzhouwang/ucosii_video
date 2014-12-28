/*     
 *  commands.c  
 *  the genie shell command part for ucosII  
 *  under skyeye  
 *  
 *  Bugs report:     Yang Ye  ( yangye@163.net )  
 *  Last modified:   2003-02-19   
 *  
 */   
   
#include "commands.h"   
/***************************new**********************************/   
#include "uart.h"
/***************************new**********************************/   

#define  OS_USING_DEBUG
#ifdef OS_USING_DEBUG
#define kprintf		printf
#else
#define kprintf(...)		
#endif

#define ERRORCOMMAND    	255  	//命令错误
#define MaxLenComBuf    		100  	//命令缓存大小

command ShellComms[MAX_COMMAND_NUM];   
char 	*argv[10];   
INT8U argc;   
INT8U CommandAnalys(char *Buf);   


/******************************************************************************************
* shell　移植接口
*/

/**
  * @brief  shell 从终端获取一个字节	
  * @param  ch 字节数据的地址				
  * @retval 0 没有数据							
	* 				 1 有数据								
  */
static INT8U shell_get_char(INT8U *ch)
{
	return uart_get_char(ch);
}

/**
  * @brief  shell 输出一个字节
  * @param  ch 字节数据
  * @retval None
  */
static void shell_put_char(INT8U ch)
{
	uart_put_char(ch);
}


/******************************************************************************************
*	shell 功能函数
*
*/

/**
  * @brief  命令导出函数接口
  * @param  CommandFunc  命令功能处理函数
	* @param  Name 命令名称
	* @param  help 命令的帮助信息
  * @retval None
  */
void command_export(INT8U (*CommandFunc)(INT8U argc,char **argv),const char *Name,const char *help)
{
	INT8U i;

	if(ShellComms[ShellComms[0].num].num >= MAX_COMMAND_NUM)
	{
		printf("Command number is full plaese change MAX_COMMAND_NUM define \n\r");
	}
	ShellComms[ShellComms[0].num].name = (char *)Name;
	ShellComms[ShellComms[0].num].help = (char *)help;
	ShellComms[ShellComms[0].num].CommandFunc = CommandFunc;
	ShellComms[0].num++;
	for(i=1;i<ShellComms[0].num;i++)
	{
		ShellComms[i].num = ShellComms[0].num;
	}
}


/**
  * @brief  显示shell中的所有命令
  * @param  argc 参数个数
	* @param  argv 参数1 参数2 ...
  * @retval None
  */
static INT8U list_command(INT8U argc,char **argv)   
{   
    int i;   
    
	for(i=0;i<ShellComms[0].num;i++)
	{
		printf("%s -- %s \n\r",ShellComms[i].name,ShellComms[i].help);
	}
	
    return 0;   
}   
            
 
/**
  * @brief  初始化命令对象				
  * @param  argc 参数个数					
	* @param  argv 参数1 参数2 ...	
  * @retval None
  */  
static INT8U InitCommands(void)   
{   
    ShellComms[0].num = 0;   
    command_export(list_command,"list","Show all shell commands");
  
    return 0;   
}   


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////   

#ifndef SHELL_TASK_STK_SIZE
#define SHELL_TASK_STK_SIZE 		128 //堆栈
#endif

#ifndef SHELL_TASK_PRIO
#define SHELL_TASK_PRIO        		7		//优先级
#endif	

static void shell_task_entry(void *Id) ;            //Main_Test_Task   
static OS_STK task_stk[SHELL_TASK_STK_SIZE];

/**
  * @brief  shell 任务初始化			
  * @param  None 参数个数					
  * @retval None
  */  
void shell_task_init(void)
{
	INT8U result;
	
   result = OSTaskCreate(shell_task_entry,(void*)0,&task_stk[SHELL_TASK_STK_SIZE - 1],SHELL_TASK_PRIO);
   if(result != OS_ERR_NONE)
   {
		kprintf("Shell Task Create Fail\n\r");
   }
}

/**
  * @brief  shell 任务			
  * @param  Id 任务参数				
  * @retval None
  */  
static void shell_task_entry(void *Id)             //Main_Test_Task   
{   

    INT8U i=0,num,err;      /*i is the pointer of commandbuf */   
    char ch;   
    char c1[1];   
    INT8U (*Func)(INT8U argc,char **argv);   
    char CommandBuf[MaxLenComBuf+1];    /*store '\0'*/      
    InitCommands();   
    CommandBuf[0] = '\0';   
   
    printf("\n\r***********************************************\n");   
    printf("\n\r*           Welcom to fighter shell           *\n");   
    printf("\n\r*           Author:fighter 2014-12-28         *\n");   
    printf("\n\r***********************************************\n\r");   
           
	/*To be done: Login & Password*/   
   
    printf("\nBEGIN UCOS SHELL,press any to continue.....");   
    shell_get_char((INT8U *)c1);   
    printf("\n\rc:\\sk>");   
    while(1)
    {   
        do
        {                
        	/* only accept a-z,0-9,A-Z,.,space,/,-   */
			err = shell_get_char((INT8U *)c1);   
			if(err == 1)
			{
				ch=c1[0];   
			}
			else
			{
				OSTimeDly(1);
				ch = 0;
			}
        }while(!((ch>='0'&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='.')||(ch==' ')||(ch=='-')||(ch=='/')||(ch=='\r')||(ch=='\b')||(ch==',')));   
           
        switch(ch)   
        {   
			case '\r':              //enter   
			{
				if (i==0)
				{                              //commandbuf is null,begin a new line   
					printf("\n\rc:\\sk>");     
				}   
				else
				{   
					if(CommandBuf[i-1]==' ')    
					{
						i--;  
					}
				   //get rid of the end space   
					CommandBuf[i] = '\0';   
					//printf("\n\rThe command is %s",CommandBuf);   
					num = CommandAnalys(CommandBuf);    //analys the argv in the commandbuf   
					if(num==ERRORCOMMAND)
					{              //error or none exist command   
						i = 0;   
						CommandBuf[i] = '\0';   
						//printf("\n\rError command is %s",CommandBuf);   
						printf("\n\rBad command or filename.");   
						printf("\n\rc:\\sk>");    
					}   
					else
					{   
						Func = ShellComms[num].CommandFunc; //call corresponding CommandFunc   
						Func(argc,argv);   
						i = 0;   
						CommandBuf[i] = '\0';   
						printf("\n\rc:\\sk>");   
					}   
				}   
				break;   
			}
        case '\b':              //backspace   
        {
			if ( i==0 )				//has backed to first one   
			{        	
				//do nothing   
			}   
			else
			{   
				i--;            //pointer back once   
				shell_put_char('\b');      //cursor back once   
				shell_put_char(' ');         //earse last char in screen   
				shell_put_char('\b');      //cursor back again   
				//moverel(-1,0);   
			}   
			break;   
        }       
        case ' ':               //don't allow continuous or begin space(' ')   
        {
			if((CommandBuf[i-1] == ' ')||(i==0)||(i>MaxLenComBuf))
			{   
				//do nothing   
			}   
			else
			{   
				CommandBuf[i] = ch;   
				i++;   
				shell_put_char(ch);  //display and store ch   
			}   
			break;   
        }
        default:                //normal key   
        {
			if (i>MaxLenComBuf)//the buf reached MAX    
			{ 
				//do nothing   
			}              
			else
			{   
  				CommandBuf[i] = ch;   
  				i++;   
  				shell_put_char(ch);  //display and store ch   
			}   
       	break;   
        }
       }     
       OSTimeDly(OS_TICKS_PER_SEC/100);   
    }//for(;;)   
}   

/**
  * @brief  shell 解析函数			
  * @param  buf 命令	
  * @retval None
  */  
INT8U CommandAnalys(char *Buf)   
{   
    INT8U i;   
    INT8U pointer;   
    INT8U num;   
    char name[20];      //command name length <20   
   
    argc = 0;              //argc is global   
    pointer = 0;   
    num = 0;   
    printf("\n\r");   
   
    while((Buf[pointer]!=' ') && (Buf[pointer]!='\0') && pointer<20 )
    {   
        name[pointer]=Buf[pointer];   
        pointer++;   
    }   
    name[pointer] = '\0';   //now got the command name, and pointer is to the first space in the Buf   
   
    for(i=0;i<MAX_COMMAND_NUM;i++)
    {   
        if(!strcmp(name,ShellComms[i].name))
        {   
            num = i;   
            break;   
        }               //find the command number   
    }                      

	//not find it   
    if (i==MAX_COMMAND_NUM) return ERRORCOMMAND;   
                       
    while(Buf[pointer]!='\0')
    {   
        if(Buf[pointer]==' ')
        {   
            if(argc>0)
            {   
                Buf[pointer] = '\0';            //end of last argv   
            }   
            pointer++;   
            argv[argc] = &Buf[pointer];    //add a parameter for every space   
            argc++;   
        }   
        else
        {   
            pointer++;   
        }   
    }
   
    return num;   
} 
