/*
 * shell_tasks.c
 *
 *  Created on: 2023 Aug 11
 *      Author: ruff
 */
#include "at32f415_board.h"

struct SHELL_TASK tasks[] = { { name: "ipconfig", send_script: send_ifconfig,
		parse_feedback: parse_ifconfig } };

void loop_tasks() {
	int i = 0;
	while (1) {
		printf("Run task-%d\r\n", i);
		printf("name: %s\r\n", tasks[i].name);

		tasks[i].send_script();

		tasks[i].parse_feedback();

		delay_sec(TASKS_DELAY_SEC);

		i++;

		if (i >= SHELL_TASKS_NUM) {
			i = 0;
		}
	}
}

void start_shell() {

	delay_sec(3);

	// send_to_console_0D0A();
	read_from_console();

	// send_to_console_0D0A();
	// oled_display_login();
	if (read_from_console() == CONSOLE_RX_TIMEOUT) {
		// oled_display_login_timeout();
		printf("Err: wait login prompt timeout\r\n");
		// goto end;
	}
	printf("check login\r\n");
	// check_login_prompt();

	delay_sec(1);

	printf("check login again\r\n");
	send_to_console_0D0A();
	//  read_from_console();
	oled_display_login();
	if (read_from_console() == CONSOLE_RX_TIMEOUT) {
		oled_display_login_timeout();
		printf("Err: wait login prompt timeout\r\n");
		goto end;
	}
	printf("check login prompt\r\n");
	if (check_login_prompt() != 0) {
		oled_display_login_wrong();
		printf("Err: login prompt wrong\r\n");
		goto end;
	}

	// check username
	printf("send user\r\n");
	oled_display_username();
	send_to_console_username();
	if (read_from_console() == CONSOLE_RX_TIMEOUT) {
		oled_display_username_timeout();
		printf("Err: wait username input timeout\r\n");
		goto end;
	}
	printf("check username prompt\r\n");
	if (check_passwd_prompt() != 0) {
		oled_display_username_wrong();
		printf("Err: username prompt wrong\r\n");
		goto end;
	}
	printf("passwd prompt OK\r\n");

	// check password
	printf("send passwd\r\n");
	oled_display_passwd();
	send_to_console_passwd();
	if (read_from_console() == CONSOLE_RX_TIMEOUT) {
		oled_display_passwd_timeout();
		printf("Err: wait passwd resp timeout\r\n");
		goto end;
	}
	printf("check cmd line prompt\r\n");
	if (check_cmdline_prompt() != 0) {
		oled_display_cmdline_wrong();
		printf("Err: cmdline prompt wrong\r\n");
		goto end;
	}
	printf("cmdline prompt OK\r\n");
	oled_display_cmdline();

	delay_ms(2000);
	oled_display_tasks();
	printf("running tasks\r\n");

	loop_tasks();

end:

	while (1) {
		at32_led_toggle(LED2);
		delay_ms(1000);
	}

}


