#include "compile_define.h"
#include "stm32f0xx.h"
#include "_hardware.h"


typedef enum
{
	IDLE = 0x00,
	PAUSE = 0x80,
	RUNNING = 0x81,
	STOP = 0x82
}TIMER_STATUS;							//use the bit 7 as the timer occupation flag

#define CFG_TIMER_CNT 4

u16 xdata _global_clock;
u8  xdata _timer_control[CFG_TIMER_CNT];		//
u32 xdata _timer_value[CFG_TIMER_CNT];


/*******************************************************************************
* Function Name  : init_timer()
* Description    : Initialize timer related registers;
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void init_timer(void)
{
	unsigned char i;

	_global_clock = 0;											//Tick = 1ms
	
	/*------------------------------------
	Timer data structure initialization
	*-----------------------------------*/
	for(i=0;i<CFG_TIMER_CNT;i++)
	{
		_timer_control[i] = IDLE;
		_timer_value[i] = 0;
	}
	
	init_timer_hardware();
}



/*******************************************************************************
* Function Name  : timer_int_svr()
* Description    : 	in timer svr, such task are performed:
					1. maintain a 16-bit clock variant _global_clock;
					2. drive system timer .
					3. drive phy & dll receiving procedure;
* Input          : None
* Output         : None
* Return         : 
*******************************************************************************/
void timer_int_svr(void)
{
	unsigned char i;
	
    led_timer_int_on();
    /* Global Clock */
    _global_clock++;
    if((u8)_global_clock==0)
    {
    }

    /* Timer Control */
	for(i=0;i<CFG_TIMER_CNT;i++)
	{
		if(_timer_control[i] == RUNNING)
		{
			if(_timer_value[i]>0)
			{
				_timer_value[i]--;
			}
			else
			{
				_timer_control[i] = STOP;
			}
		}
	}
    led_timer_int_off();
}

u16 get_global_clock16(void)
{
	u16 val;
	
	//ENTER_CRITICAL();
	val = _global_clock;
	//EXIT_CRITICAL();
	
	return val;
}


