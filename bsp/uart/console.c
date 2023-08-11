/*
 * console.c
 *
 *  Created on: 2023 Aug 11
 *      Author: ruff
 */
#include <string.h>

#include "at32f415_board.h"

uint8_t console_rx_buffer[UART2_BUFFER_LEN];
uint16_t console_rx_len = 0;
uint8_t username[32];
uint8_t userpasswd[64];

char console_tx_buffer[256];

void console_init(){
	sprintf(username, "%s","ruff");
	sprintf(userpasswd, "%s", "nanchao.org");
}

/* read something from VT102 console in n seconds.
 *
 */
int read_from_console(void){
	int counter = 0;
	while(counter++ < CONSOLE_WAIT_TIMES){
		delay_ms(CONSOLE_WAIT_INTERVAL);
		if(uart2_get_rx_flag() == 1){
			uart2_reset_rx_flag();
			return CONSOLE_RX_OK;
		}
	}
	return CONSOLE_RX_TIMEOUT;
}
void send_to_console(char* buf, int len){
	uart2_tx_send((uint8_t*)buf, len);
}
void send_to_console_0D0A(){
	console_tx_buffer[0] = 0x0D;
	console_tx_buffer[1] = 0x0A;
	console_tx_buffer[2] = 0x0;
	send_to_console(console_tx_buffer, strlen(console_tx_buffer));
}

int check_login_prompt(){
	printf("login: %d\n", console_rx_len);
	for(int i = 0; i < console_rx_len; i++){
		char ch = console_rx_buffer[i];
		if((ch >= 0x40 && ch <= 0x7E) ||
				(ch == 0x0D) ||
				(ch == 0x0A) ||
				(ch >= 0x20 && ch <= 0x3E ))
		{
			printf("%c",ch);
        }
	}
	printf("\r\n");
}
int check_passwd_prompt(){
	printf("login: %d\n", console_rx_len);
}
int check_cmdline_prompt(){
	printf("login: %d\n", console_rx_len);
}
