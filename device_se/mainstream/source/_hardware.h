void init_timer_hardware(void);
void init_usart2_hardware(void);
void init_gpio(void);
void uart_send8(u8 byte_data);
void usart2_int_entry(void);

u8 read_spi(u8 addr);
void write_spi(u8 addr, u8 value);
void init_spi(void);
u8 switch_spi(u8 value);
void exti_int_entry(void);



void led_timer_int_on(void);
void led_timer_int_off(void);
void led_main_loop_flash(void);


void init_measure_com_hardware(void);
void measure_com_send8(u8 byte_data);
u8 measure_com_read8(u8 * rec_byte);


