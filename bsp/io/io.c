/*
 * io.c
 *
 *  Created on: 2023 Aug 29
 *      Author: ruff
 */


#include "at32f415_board.h"

void init_gpio(
		crm_periph_clock_type crm_clock,
		gpio_type* port,
		uint16_t pin,
		gpio_pull_type pull,
		gpio_mode_type mode){

	gpio_init_type gpio_init_struct;

	/* enable the led clock */
	crm_periph_clock_enable(crm_clock, TRUE);

	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);

	/* configure the led gpio */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = mode;
	gpio_init_struct.gpio_pins = pin;
	gpio_init_struct.gpio_pull = pull;
	gpio_init(port, &gpio_init_struct);
}

void init_gpio_output(
		crm_periph_clock_type crm_clock,
		gpio_type* port,
		uint16_t pin,
		gpio_pull_type pull){

	init_gpio(crm_clock, port, pin, pull, GPIO_MODE_OUTPUT);
}

void init_gpio_input(
		crm_periph_clock_type crm_clock,
		gpio_type* port,
		uint16_t pin,
		gpio_pull_type pull){
	init_gpio(crm_clock, port, pin, pull, GPIO_MODE_INPUT);
}
void gpio_on(gpio_type* port, uint16_t pin){
	port->scr = pin;
}
void gpio_off(gpio_type* port, uint16_t pin){
	port->clr = pin;
}
void gpio_toggle(gpio_type* port, uint16_t pin){
	port->odt ^= pin;
}
uint8_t gpio_read(gpio_type* port, uint16_t pin){
	if(gpio_input_data_bit_read(port, pin)== RESET){
		return 0;
	}else{
		return 1;
	}
}

void usb_detect_init(void){
	init_gpio_input(
			USB_DETECT_GPIO_CLK,
			USB_DETECT_GPIO_PORT,
			USB_DETECT_PIN,
			GPIO_PULL_UP);
}


