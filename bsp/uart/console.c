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
char cmdline_rx_buffer[256];
uint16_t cmdline_rx_len=0;

const char* ifconfig = "ifconfig | grep \"inet \" | awk -F\' \' \'{print $2}\' | grep -v \"172\" | grep -v \"127\" | xargs -n 1 -d\'\n\'";
//const char* ifconfig = "ifconfig | grep \"inet \" | awk -F\' \' \'{print $2}\' | grep -v \"172\" | grep -v \"127\" | xargs -n 1 ";

uint8_t   data_buffer[256];
uint16_t  data_buffer_index = 0;

void console_init(){
	sprintf(username, "%s","ruff");
	sprintf(userpasswd, "%s", "nanchao.org");
}
/*
 * only read the 1st string
 * the 2nd string is the cmd line prompt
 */
int read_from_cmdline(void){
	int counter = 0;
	int first_fb_saved = 0;
	cmdline_rx_len = 0;

	while(counter++ < 200){

		if(uart2_get_rx_flag() == 1){
			for(int i = 0; i< console_rx_len; i++){
				cmdline_rx_buffer[cmdline_rx_len++] = console_rx_buffer[i];
			}

			uart2_reset_rx_flag();
			// return CONSOLE_RX_OK;
		}
		delay_ms(5);
	}
	if(cmdline_rx_len > 10){
		return CONSOLE_RX_OK;
	}else{
		return CONSOLE_RX_TIMEOUT;
	}

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
void send_to_console_username(){
	sprintf( console_tx_buffer, "%s\r\n", username);
	send_to_console(console_tx_buffer, strlen(console_tx_buffer));
}
void send_to_console_passwd(){
	sprintf( console_tx_buffer, "%s\r\n", userpasswd);
	send_to_console(console_tx_buffer, strlen(console_tx_buffer));
}
void send_ifconfig(void){
	printf("send ipconfig\r\n");
	sprintf( console_tx_buffer, "%s\r\n", ifconfig);
	send_to_console(console_tx_buffer, strlen(console_tx_buffer));
}

void print_rx_buffer(){
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
void print_cmdline_buffer(){
	printf("cmdline_buffer %d\r\n", cmdline_rx_len);
	for(int i = 0; i < cmdline_rx_len; i++){
		char ch = cmdline_rx_buffer[i];
		if((ch >= 0x40 && ch <= 0x7E) ||
				(ch == 0x0D) ||
				(ch == 0x0A) ||
				(ch >= 0x20 && ch <= 0x3E ))
		{
			printf(" %c 0x%02x ",ch, ch);
        }
	}
	printf("\r\n-------------------\r\n");

	for(int i = 0; i < cmdline_rx_len; i++){
		char ch = cmdline_rx_buffer[i];
		if((ch >= 0x40 && ch <= 0x7E) ||
				(ch == 0x0D) ||
				(ch == 0x0A) ||
				(ch >= 0x20 && ch <= 0x3E ))
		{
			printf(" %c 0x%02x ",ch);
        }else{
        	printf("\r\n0x%02x\r\n",ch);
        }
	}
	printf("\r\n");

}
void print_buffer(uint8_t *buffer, uint16_t len){
	printf("\r\nprint buffer %d\r\n", len);

	for(int i = 0; i < len; i++){
		char ch = buffer[i];
		printf("%c",ch);
	}
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
	// check login:SP,
	if(console_rx_buffer[console_rx_len -1] == 0x20 &&
			console_rx_buffer[console_rx_len - 2] == 0x3A &&
			console_rx_buffer[console_rx_len - 3] == 0x6E){
		return 0;
	}else{
		return -1;
	}

}
int check_passwd_prompt(){
	printf("passwd prompt : %d\n", console_rx_len);
	print_rx_buffer();
	// check passwd:
	if(console_rx_buffer[console_rx_len -1] == 0x20 &&
			console_rx_buffer[console_rx_len - 2] == 0x3A &&
			console_rx_buffer[console_rx_len - 3] == 0x64){
		return 0;
	}else{
		return -1;
	}
}
int check_cmdline_prompt(){
	printf("cmdline: %d\n", console_rx_len);
	print_rx_buffer();
	// check cmdline:

	return 0;
}
int is_valid_character(uint8_t ch){
	if((ch >= 0x40 && ch <= 0x7E)||
					(ch == 0x0D) ||
					(ch == 0x0A) ||
					(ch >= 0x20 && ch <= 0x3E )){
		return 0;
	}else{
		return -1;
	}
}
void remove_invisible_character(uint8_t* buf, uint16_t *len){
	int last_index = 0;

	for(int i = 0; i < *len; i++){
		if(is_valid_character(buf[i]) == 0){
			buf[last_index++] = buf[i];
		}
	}
	buf[last_index] = 0x0;
	*len = last_index;
}
/*
 * state 0: init,
 * state 1: met 1st 0x0D
 * state 2:
 * state 3:
 * state 4:
 */
/*
int parse_ifconfig_valid_data(){
	int state = 0;
	uint8_t ch, ch_next;

	for(int i = 0; i < cmdline_rx_len; i++){
		ch = cmdline_rx_buffer[i];
		if( i <= (cmdline_rx_len - 2)){
			ch_next = cmdline_rx_buffer[i + 1];
		}

		if(is_valid_character(ch) != 0){
			continue;
		}

		switch(state){
		case 0:
			if(ch == 0x0D){
				state = 1;
			}
			break;
		case 1:
			if(ch == 0x0A){
				state = 2;
			}else{
				printf("Err: wrong character met in state 1.\r\n");
				goto ifconfig_parse_fail;
			}
			break;
		case 2:
			if(ch == '1'){
				data_buffer[data_buffer_index++] = ch;
				state = 3;
			}else{

			}
			break;
		}
	}
	return 0;
ifconfig_parse_fail:
	return -1;
}

*/
int is_num(char ch){
	if( ch >= '0' && ch <= '9' ){
		return 0;
	}else{
		return -1;
	}
}
int is_num_or_dot(char ch){
	if(ch == '.' || ( ch >= '0' && ch <= '9')){
		return 0;
	}else{
		return -1;
	}
}
void parse_ifconfig_valid_ip(char* buffer, uint16_t* plen){
	if( is_num(buffer[0]) == 0 &&
			is_num(buffer[1]) == 0 &&
			is_num_or_dot(buffer[2]) == 0 &&
			is_num_or_dot(buffer[3]) == 0){
		printf("print ip %d\r\n", *plen);
		// remove unprintable character,
		remove_invisible_character(buffer, plen);

		oled_display_string(buffer);

		delay_ms(2000);
	}
}

/**
 *  \r\n found,
 */
int parse_ifconfig_valid_data(char* buffer, uint16_t len){
	data_buffer_index = 0;
	for(int i = 0; i < len; i++){
		// last line, command line,

		data_buffer[data_buffer_index++] = buffer[i];
		if( i == len-2 ){
			printf("last line\r\n");
			data_buffer[data_buffer_index++] = buffer[i+1];
			data_buffer[data_buffer_index] = 0x0;
			printf("%s\r\n", data_buffer);
			break;
		}
		// found the line end mark character
		if(buffer[i] == 0x0D || buffer[i] == 0x0A){
//			data_buffer[data_buffer_index] = 0x0;
//			printf("%s\n", data_buffer);
			// if it is the IP address print it out
//			data_buffer_index = 0;
			data_buffer[data_buffer_index] = 0x0;
			data_buffer_index--;

			printf("Got a line =>\r\n");
			printf("==%s==\r\n", data_buffer);
			// print ip out
			parse_ifconfig_valid_ip(data_buffer, &data_buffer_index);

			i++;
			while( buffer[i] == 0x0D || buffer[i] == 0x0A ){
				i++;
			}
			i--;
			data_buffer_index = 0;
		}
	}

}
void parse_ifconfig(void){
	int i = 0;

	printf("parse ifconfig\r\n");
	if(read_from_cmdline() == CONSOLE_RX_TIMEOUT){
		oled_display_task_timeout();
		printf("Err: wait task fb timeout\r\n");

	}else{
		printf("ifconfig feedback:\r\n");
		print_cmdline_buffer();

		// parse ip address out,
		if(cmdline_rx_len > 10){
			remove_invisible_character(cmdline_rx_buffer, &cmdline_rx_len);
			printf("after remove invisible character\r\n");
			print_buffer(cmdline_rx_buffer, cmdline_rx_len);

			parse_ifconfig_valid_data(cmdline_rx_buffer, cmdline_rx_len);
		}else{
			printf("Err: parse cmdline info failed\r\n");
		}


	}
}
