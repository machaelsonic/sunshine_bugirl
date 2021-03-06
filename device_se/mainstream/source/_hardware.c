#include "compile_define.h"
#include "stm32f0xx.h"
#include "_uart.h"

#define PLC_SPI					SPI2
#define	PLC_SPI_GPIO_PORT		GPIOB
#define PLC_SPI_SCS_PIN			GPIO_Pin_12
#define PLC_SPI_SCK_PIN			GPIO_Pin_13
#define PLC_SPI_SCK_PIN_SOURCE	GPIO_PinSource13
#define PLC_SPI_MISO_PIN		GPIO_Pin_14
#define PLC_SPI_MISO_PIN_SOURCE	GPIO_PinSource14
#define PLC_SPI_MOSI_PIN		GPIO_Pin_15
#define PLC_SPI_MOSI_PIN_SOURCE	GPIO_PinSource15

#define PLC_SPI_INT_GPIO_PORT	GPIOA
#define PLC_SPI_INT_PIN			GPIO_Pin_8
#define PLC_SPI_INT_EXTI_LINE	EXTI_Line8
#define PLC_SPI_INT_IRQC		EXTI8IRQn



#define PLC_SPI_SCS_LOW()		(PLC_SPI_GPIO_PORT->BRR=PLC_SPI_SCS_PIN)
#define PLC_SPI_SCS_HIGH()		(PLC_SPI_GPIO_PORT->BSRR=PLC_SPI_SCS_PIN)


#define LED_GPIO_PORT			GPIOA
#define LED_TEST3_PIN			GPIO_Pin_5
#define LED_TEST4_PIN			GPIO_Pin_6
#define LED_TIMER_INT			LED_TEST3_PIN
#define LED_MAIN_LOOP			LED_TEST4_PIN



#define BL6810_CTRL_GPIO_PORT	GPIOB
#define BL6810_MODE_PIN			GPIO_Pin_3
#define BL6810_RST_PIN			GPIO_Pin_4
#define BL6810_TXON_PIN			GPIO_Pin_5

#define BL6523_GPIO_PORT		GPIOB
#define BL6523_RST_PIN			GPIO_Pin_6





/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  None
  * @retval None
  */
void init_timer_hardware(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 239;
	TIM_TimeBaseStructure.TIM_Prescaler = 199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);

}

void led_on(uint16_t pin)
{
	GPIO_ResetBits(LED_GPIO_PORT,pin);
}

void led_off(uint16_t pin)
{
	GPIO_SetBits(LED_GPIO_PORT,pin);
}


void toggle_gpio_pin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if(GPIOx->ODR & GPIO_Pin)
	{
		GPIOx->BRR = GPIO_Pin;
	}
	else
	{
		GPIOx->BSRR = GPIO_Pin;
	}
	
}

void led_flash(uint16_t pin)
{
	toggle_gpio_pin(LED_GPIO_PORT,pin);
}

void led_timer_int_on(void)
{
	led_on(LED_TIMER_INT);
}

void led_timer_int_off(void)
{
	led_off(LED_TIMER_INT);
}

void led_main_loop_flash(void)
{
	led_flash(LED_MAIN_LOOP);
}

/*******************************************************************************
* Function Name  : init_gpio()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void init_gpio(void)
{
	volatile u32 i,j;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = LED_TEST3_PIN | LED_TEST4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LED_GPIO_PORT, LED_TEST3_PIN | LED_TEST4_PIN);

	GPIO_InitStructure.GPIO_Pin = BL6810_MODE_PIN | BL6810_RST_PIN | BL6810_TXON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BL6810_CTRL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BL6523_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(BL6523_GPIO_PORT, &GPIO_InitStructure);

	GPIO_SetBits(BL6810_CTRL_GPIO_PORT, BL6810_TXON_PIN);
	GPIO_ResetBits(BL6810_CTRL_GPIO_PORT, BL6810_MODE_PIN | BL6810_RST_PIN);
	GPIO_ResetBits(BL6523_GPIO_PORT, BL6523_RST_PIN);

	for(i=0;i<4;i++)
	{
		for(j=0;j<599999UL;j++);
		led_flash(LED_TEST3_PIN);
        led_flash(LED_TEST4_PIN);
	}
	GPIO_SetBits(BL6810_CTRL_GPIO_PORT, BL6810_RST_PIN);	
	GPIO_SetBits(BL6523_GPIO_PORT, BL6523_RST_PIN);
	
}

void init_usart2_hardware(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);	//USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART Rx as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* USART时钟 */
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART2, &USART_ClockInitStructure );
	
	/* Uart口配置 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure );
	
	/* 设置,使能 */
	USART_Cmd(USART2, ENABLE);

	/* Uart接收中断使能 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable the USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void uart_send8(u8 byte_data)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, byte_data);
}




#define UART_PORT USART2
void usart2_int_entry(void)
{
	u8 byte_data;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		byte_data = USART_ReceiveData(USART2);
		uart_rcv_int_svr(byte_data);
	}
	else
	{
		my_printf("error int:");
		if(USART_GetITStatus(UART_PORT, USART_IT_PE) != RESET)
		{
			my_printf("USART_IT_PE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_PE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_TXE) != RESET)
		{
			my_printf("USART_IT_TXE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_TXE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_TC) != RESET)
		{
			my_printf("USART_IT_TC\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_TC);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_RXNE) != RESET)
		{
			my_printf("USART_IT_RXNE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_RXNE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_IDLE) != RESET)
		{
			my_printf("USART_IT_IDLE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_IDLE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_LBD) != RESET)
		{
			my_printf("USART_IT_LBD\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_LBD);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_CTS) != RESET)
		{
			my_printf("USART_IT_CTS\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_CTS);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_ORE) != RESET)
		{
			my_printf("USART_IT_ORE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_ORE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_NE) != RESET)
		{
			my_printf("USART_IT_NE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_NE);
		}
		else if(USART_GetITStatus(UART_PORT, USART_IT_FE) != RESET)
		{
			my_printf("USART_IT_FE\r\n");
			USART_ClearITPendingBit(UART_PORT,USART_IT_FE);
		}
		else
		{
			if (USART_GetFlagStatus(UART_PORT, USART_FLAG_ORE) != RESET)//注意！不能使用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断
		    {
				my_printf("over run\r\n");
		        USART_ReceiveData(UART_PORT);
    		}
		}
	}
}

/*******************************************************************************
* Function Name  : init_spi
* Description    : Initialize SPI INT(PA8)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void init_exti(void)
{
	GPIO_InitTypeDef   	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PLC_SPI_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(PLC_SPI_INT_GPIO_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = PLC_SPI_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*******************************************************************************
* Function Name  : exti_int_entry
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void exti_int_entry(void)
{
	uart_send8('M');
}


/*******************************************************************************
* Function Name  : init_spi
* Description    : Initialize STM32 SPI1 Port (comm to BL6810)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void init_spi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	// 给时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	// 配IO
	GPIO_InitStructure.GPIO_Pin = PLC_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(PLC_SPI_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PLC_SPI_MISO_PIN;
	GPIO_Init(PLC_SPI_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PLC_SPI_MOSI_PIN;
	GPIO_Init(PLC_SPI_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PLC_SPI_SCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PLC_SPI_GPIO_PORT, &GPIO_InitStructure);

	PLC_SPI_SCS_HIGH();

	GPIO_PinAFConfig(PLC_SPI_GPIO_PORT, PLC_SPI_SCK_PIN_SOURCE, GPIO_AF_0);
	GPIO_PinAFConfig(PLC_SPI_GPIO_PORT, PLC_SPI_MISO_PIN_SOURCE, GPIO_AF_0);
	GPIO_PinAFConfig(PLC_SPI_GPIO_PORT, PLC_SPI_MOSI_PIN_SOURCE, GPIO_AF_0);

	
	// 配置SPI
	SPI_I2S_DeInit(PLC_SPI);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(PLC_SPI, &SPI_InitStructure);

	 /* Initialize the FIFO threshold */
	SPI_RxFIFOThresholdConfig(PLC_SPI, SPI_RxFIFOThreshold_QF);

	/* Enable SPI1  */
	SPI_Cmd(PLC_SPI, ENABLE);

	/* Enable EXTI  */
	init_exti();
}

/*******************************************************************************
* Function Name  : spi_switch
* Description    : Swith a 8-byte info with SPI Slave
* Input          : Byte which is to delivered to slave
* Output         : Byte which is returned from slave
* Return         : Byte which is returned from slave
*******************************************************************************/
u8 switch_spi(u8 value)
{
	u32 awd = 0;
	/* Loop while DR register in not emplty */
	do
	{
		if(awd++>50000UL)
		{
			my_printf("spi tx lock!\n");
			return 0;
		}
	}
	while(SPI_I2S_GetFlagStatus(PLC_SPI, SPI_I2S_FLAG_TXE) == RESET); 

	/* Send value through the SPI1 peripheral */	
	SPI_SendData8(PLC_SPI, value);	

	/* Wait to receive a value */
	awd = 0;
	do
	{
		if(awd++>50000UL)	//anti deadloop
		{
			my_printf("spi rx lock!\n");
			return 0;
		}
	}
	while(SPI_I2S_GetFlagStatus(PLC_SPI, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the value read from the SPI bus */  
	value = SPI_ReceiveData8(PLC_SPI); 

	return value;

}

/*******************************************************************************
* Function Name  : spi_write
* Description    : Write a byte to BL6810
* Input          : ch, address to wrote, byte to wrote in
* Output         : None
* Return         : None
*******************************************************************************/
void write_spi(u8 addr, u8 value)
{
	addr &= 0x7F;
//	__RESTORE_AND_SET_PRIMASK();
	ENTER_CRITICAL();

	PLC_SPI_SCS_LOW();
	switch_spi(addr);
	switch_spi(value);
	PLC_SPI_SCS_HIGH();

	EXIT_CRITICAL();
}

/*******************************************************************************
* Function Name  : spi_read
* Description    : Read a byte info from SPI Slave
* Input          : ch, addr
* Output         : None
* Return         : byte read from SPI Slave
*******************************************************************************/
u8 read_spi(u8 addr)
{
	u8 byte = 0;

//	__RESTORE_AND_SET_PRIMASK();
	ENTER_CRITICAL();					//2014-12-18 保证SPI操作为原子操作
	
	addr |= 0x80;
	{
		PLC_SPI_SCS_LOW();
		switch_spi(addr);
		byte = switch_spi(0xFF);
		PLC_SPI_SCS_HIGH();
	}


//	__RECOVER_PRIMASK();
	EXIT_CRITICAL();
	return byte;
}


#define MEASURE_COM						USART1
#define MEASURE_COM_GPIO				GPIOA
#define MEASURE_COM_TXD_PIN				GPIO_Pin_10
#define MEASURE_COM_RXD_PIN				GPIO_Pin_9
#define MEASURE_COM_TXD_PIN_SOURCE		GPIO_PinSource10
#define MEASURE_COM_RXD_PIN_SOURCE		GPIO_PinSource9
#define MEASURE_COM_READ_TIMEOUT		6999UL

/**
  * @brief  initialize usart1
  * @param  
  * @note   
  * @retval None
  */
void init_measure_com_hardware(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
  	USART_ClockInitTypeDef  USART_ClockInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(MEASURE_COM_GPIO, MEASURE_COM_TXD_PIN_SOURCE, GPIO_AF_1);	//USART1
	GPIO_PinAFConfig(MEASURE_COM_GPIO, MEASURE_COM_RXD_PIN_SOURCE, GPIO_AF_1);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = MEASURE_COM_RXD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MEASURE_COM_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MEASURE_COM_TXD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(MEASURE_COM_GPIO, &GPIO_InitStructure);
	
	/* USART时钟 */
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(MEASURE_COM, &USART_ClockInitStructure );
	
	/* Uart口配置 */
	USART_InitStructure.USART_BaudRate = 4800;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(MEASURE_COM, &USART_InitStructure );
	
	///* 设置,使能 */
	USART_Cmd(MEASURE_COM, ENABLE);
}

/**
  * @brief  Send a byte to measure serial com
  * @param  
  * @note   
  * @retval None
  */
void measure_com_send8(u8 byte_data)
{
	u32 timeout=0;

	while(USART_GetFlagStatus(MEASURE_COM, USART_FLAG_TXE) == RESET)
	{
		if(timeout++>99999UL)
		{
			my_printf("measure com xmt hardfault\n");
			return;
		}
	}
	USART_SendData(MEASURE_COM, byte_data);
}

/**
  * @brief  Read a byte from measure serial com
  * @param  
  * @note   
  * @retval "1" when got a byte, "0" when timeout
  */
u8 measure_com_read8(u8 * pt_rec_byte)
{
	u16 timer = 0;

	for(timer=0;timer<MEASURE_COM_READ_TIMEOUT;timer++)
	{
		if(USART_GetFlagStatus(MEASURE_COM, USART_FLAG_RXNE) != RESET)
		{
			*pt_rec_byte = USART_ReceiveData(MEASURE_COM);
//led_main_loop_flash();

			return 1;
		}
	}
//led_main_loop_flash();

	return 0;
}

