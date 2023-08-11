/*
 * shell_tasks.c
 *
 *  Created on: 2023 Aug 11
 *      Author: ruff
 */
#include "at32f415_board.h"


struct SHELL_TASK tasks[] = {
		{
				name: "ipconfig",
				send_script: send_ifconfig,
				parse_feedback: parse_ifconfig
		}
};

void loop_tasks(){
	int i = 0;
	while(1){
		printf("Run task-%d\r\n", i);
		printf("name: %s\r\n", tasks[i].name);

		tasks[i].send_script();

		tasks[i].parse_feedback();

		delay_sec(5);

		i++;

		if(i >= SHELL_TASKS_NUM){
			i = 0;
		}
	}
}
