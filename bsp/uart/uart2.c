/*
 * uart2.c
 *
 *  Created on: 2023 Aug 4
 *      Author: ruff
 */

#include "at32f415_board.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint8_t uart2_rx_buffer[UART2_BUFFER_LEN];
uint8_t uart2_rx_tail = 0;
uint8_t uart2_rx_head = 0;

uint8_t uart2_tx_buffer[UART2_BUFFER_LEN];
uint8_t uart2_tx_tail = 0;
uint8_t uart2_tx_head = 0;

uint8_t uart2_rx_flag = 0; // 1 received packet ,
uint8_t uart2_tx_flag = 0; // 1 tx packet succeed ,


void uart2_init(uint32_t baudrate){
	gpio_init_type gpio_init_struct;

	/* enable the usart2 and gpio clock */
	crm_periph_clock_enable(D2_UART_CRM_CLK, TRUE);
	crm_periph_clock_enable(D2_UART_TX_GPIO_CRM_CLK, TRUE);

	gpio_default_para_init(&gpio_init_struct);

	/* configure the usart2 tx pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = D2_UART_TX_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(D2_UART_TX_GPIO, &gpio_init_struct);

	/* configure the usart2 rx pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = D2_UART_RX_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(D2_UART_TX_GPIO, &gpio_init_struct);

	/* config usart nvic interrupt */
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(D2_UART_IRQn, 2, 0);

	usart_init(D2_UART, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(D2_UART, TRUE);
	usart_receiver_enable(D2_UART, TRUE);

	/* enable usart2 interrupt */
	usart_interrupt_enable(D2_UART, USART_RDBF_INT, TRUE);
	usart_interrupt_enable(D2_UART, USART_IDLE_INT, FALSE);
	usart_interrupt_enable(D2_UART, USART_TDBE_INT, FALSE);

	usart_enable(D2_UART, TRUE);

}

void USART2_IRQHandler(void)
{
  if(D2_UART->ctrl1_bit.rdbfien != RESET)
  {
    if(usart_flag_get(D2_UART, USART_RDBF_FLAG) != RESET)
    {
      /* read one byte from the receive data register */
      uart2_rx_buffer[uart2_rx_head++] = usart_data_receive(D2_UART);

      usart_interrupt_enable(D2_UART, USART_IDLE_INT, TRUE);

    }
    if(usart_flag_get(D2_UART, USART_IDLEF_FLAG) != RESET){
  	  // read from the rx buffer,


    	uart2_rx_flag = 1;

    	usart_interrupt_enable(D2_UART, USART_IDLE_INT, FALSE);
    }
  }

  if(D2_UART->ctrl1_bit.tdbeien != RESET)
  {
    if(usart_flag_get(D2_UART, USART_TDBE_FLAG) != RESET)
    {
      printf(".");

      /* write one byte to the transmit data register */
      usart_data_transmit(D2_UART, uart2_tx_buffer[uart2_tx_tail++]);

//      if (uart2_tx_tail >= UART2_BUFFER_LEN){
//    	  uart2_tx_tail = 0;
//      }

      if(uart2_tx_tail == uart2_tx_head)
      {
        /* disable the usart3 transmit interrupt */
        usart_interrupt_enable(D2_UART, USART_TDBE_INT, FALSE);
        uart2_tx_head = 0;
        uart2_tx_flag = 1;
      }
    }
  }
}
uint8_t uart2_get_rx_flag(){
	return uart2_rx_flag;
}
void uart2_clear_rx_flag(){
	uart2_rx_flag = 0;
	uart2_rx_head = 0;
}
uint8_t uart2_get_rx_len(){
	return uart2_rx_head;
}
uint8_t * uart2_get_rx_buf(){
	return uart2_rx_buffer;
}
void uart2_tx_send(uint8_t*buf, uint16_t len){
	uart2_tx_head = 0;
	for(int i = 0; i< len; i++){
		uart2_tx_buffer[uart2_tx_head++] = buf[i];
		if(uart2_tx_head >= UART2_BUFFER_LEN){
			break;
		}
	}
	uart2_tx_head--;

	uart2_tx_flag = 0;
	uart2_tx_tail = 0;
	usart_interrupt_enable(D2_UART, USART_TDBE_INT, TRUE);
}
uint8_t uart2_get_tx_flag(){
	return uart2_tx_flag;
}

void uart2_tx_printf(char* format, ...){
	char temp[UART2_BUFFER_LEN];

	va_list args_list;

	va_start(args_list, format);

	vsprintf(temp, format, args_list);

	//printf(temp);

//	for(int i = 0; i< strlen(temp); i++){
//		buffer[buffer_head++] = temp[i];
//		if(buffer_head >= SIMU_UART_BUFFER_LEN){
//			buffer_head = 0;
//		}
//	}
	uart2_tx_send(temp, strlen(temp));

	va_end(args_list);
}

