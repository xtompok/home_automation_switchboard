#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>

#define LED0_PORT GPIOD
#define LED0_PIN GPIO0

#define LED1_PORT GPIOD
#define LED1_PIN GPIO1


#define ENC1_BUT_PORT GPIOA
#define ENC1_BUT_PIN GPIO10

#define ENC2_BUT_PORT GPIOB
#define ENC2_BUT_PIN GPIO15


#define USART_CONSOLE USART1

ssize_t _write(int file, const char *ptr, ssize_t len);

ssize_t _write(int file, const char *ptr, ssize_t len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(USART_CONSOLE, '\r');
			}
			usart_send_blocking(USART_CONSOLE, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

int main(void){

	rcc_osc_on(RCC_HSE);

	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOA);

	rcc_periph_clock_enable(RCC_USART3);
	rcc_periph_clock_enable(RCC_USART1);

	rcc_periph_clock_enable(RCC_TIM1);

	usart_set_baudrate(USART3, 115200);
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_mode(USART3, USART_MODE_TX | USART_MODE_RX);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO9);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO8);
	
	usart_enable(USART3);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX | USART_MODE_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO12);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
	gpio_set_af(GPIOB, GPIO_AF0, GPIO6);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7);
	gpio_set_af(GPIOB, GPIO_AF0, GPIO7);

	USART_CR3(USART1)|=USART_CR3_DEM;


	usart_enable(USART1);

	setbuf(stdout, NULL);

	gpio_mode_setup(LED0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED0_PIN);
	gpio_mode_setup(LED1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED1_PIN);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
	gpio_set_af(GPIOA, GPIO_AF2, GPIO8);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
	gpio_set_af(GPIOA, GPIO_AF2, GPIO9);


	timer_set_period(TIM1,1024);
	timer_slave_set_mode(TIM1,TIM_SMCR_SMS_EM3);
	timer_ic_set_input(TIM1,TIM_IC1, TIM_IC_IN_TI1);
	timer_ic_set_input(TIM1,TIM_IC2, TIM_IC_IN_TI2);

	timer_enable_counter(TIM1);

	gpio_mode_setup(ENC1_BUT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ENC1_BUT_PIN);
	gpio_mode_setup(ENC2_BUT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ENC2_BUT_PIN);


	int i;
	int j;
	int c;
	j = 0;
	c = 0;

	usart_send_blocking(USART3,'3');
	usart_send_blocking(USART1,'1');

	printf("Test");

	while(1) {
		printf("Counter: %d",timer_get_counter(TIM1));
		while (gpio_get(ENC2_BUT_PORT, ENC2_BUT_PIN)==0){}
//		usart_send_blocking(USART3,usart_recv_blocking(USART1));
		gpio_toggle(LED0_PORT, LED0_PIN);
		gpio_toggle(LED1_PORT, LED1_PIN);
	}
	while (0){		/* toggle each led in turn */
		for (i = 0; i < 500000; i++) {	/* Wait a bit. */
			__asm__("nop");
		}
		gpio_toggle(LED0_PORT, LED0_PIN);
		gpio_toggle(LED1_PORT, LED1_PIN);
		for (i = 0; i < 500000; i++) {	/* Wait a bit. */
			__asm__("nop");
		}
		usart_send_blocking(USART3, c + '0'); /* USART3: Send byte. */
		c = (c == 9) ? 0 : c + 1; /* Increment c. */
		if ((j++ % 80) == 0) { /* Newline after line full. */
			usart_send_blocking(USART3, '\r');
			usart_send_blocking(USART3, '\n');
		}
		usart_send_blocking(USART1, c + '0'); /* USART3: Send byte. */
		c = (c == 9) ? 0 : c + 1; /* Increment c. */
		if ((j++ % 80) == 0) { /* Newline after line full. */
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
		}
	
	}

	return 0;
}
