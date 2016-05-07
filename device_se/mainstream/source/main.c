#include "compile_define.h"
#include "stm32f0xx.h"
#include "_timer.h"
#include "_hardware.h"
#include "_uart.h"
#include "_measure.h"


int main(void)
{
	u32 cnt = 0;
    
	init_gpio();

	init_timer();
	init_uart();

	init_spi();
	init_measure();

	my_printf("/**************************\n");
	my_printf("*****STM32F030 PV Demo*****\n");
	my_printf("**************************/\n");

	while(1)
	{
		if(cnt++>50000UL)
		{
			cnt = 0;
		}
	
		debug_uart_cmd_proc();
	}
}
