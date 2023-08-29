/*
 * config_tasks.c
 *
 *  Created on: 2023 Aug 29
 *      Author: ruff
 */

#include "at32f415_board.h"

void start_config(){
	printf("Enter config task\r\n");

	while(1){
		delay_ms(2000);
		printf("Go\r\n");
	}
}
