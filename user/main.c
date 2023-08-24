/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f415_board.h"
#include "at32f415_clock.h"

/** @addtogroup AT32F415_periph_template
  * @{
  */
#include <string.h>
/** @addtogroup 415_LED_toggle LED_toggle
  * @{
  */


#define DELAY                            500
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;


void button_exint_init(void);
void button_isr(void);

/**
  * @brief  configure button exint
  * @param  none
  * @retval none
  */
void button_exint_init(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
  * @brief  button handler function
  * @param  none
  * @retval none
  */
void button_isr(void)
{
  /* delay 5ms */
  delay_ms(5);

  /* clear interrupt pending bit */
  exint_flag_clear(EXINT_LINE_0);

  /* check input pin state */
  if(SET == gpio_input_data_bit_read(USER_BUTTON_PORT, USER_BUTTON_PIN))
  {
    if(g_speed == SLOW)
      g_speed = FAST;
    else
      g_speed = SLOW;
  }
}

/**
  * @brief  exint0 interrupt handler
  * @param  none
  * @retval none
  */
void EXINT0_IRQHandler(void)
{
  button_isr();
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
//  uint8_t *buf = uart2_get_rx_buf();
//  char  tx_buf[128];

  system_clock_config();

  at32_board_init();

  printf("\r\nHello console %s\r\n", get_version());


  oled_init();

  usb_init();

  console_init();

  oled_display_welcome();

  printf("\r\nBefore loop\r\n");

  while(1){
	  delay_sec(3);
	  printf("run\r\n");
  }

/*
  delay_sec(3);

  // send_to_console_0D0A();
  read_from_console();

  // send_to_console_0D0A();
  // oled_display_login();
  if(read_from_console() == CONSOLE_RX_TIMEOUT){
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
  if(read_from_console() == CONSOLE_RX_TIMEOUT){
	  oled_display_login_timeout();
	  printf("Err: wait login prompt timeout\r\n");
	  goto end;
  }
  printf("check login prompt\r\n");
  if(check_login_prompt() != 0){
	  oled_display_login_wrong();
	  printf("Err: login prompt wrong\r\n");
	  goto end;
  }

  // check username
  printf("send user\r\n");
  oled_display_username();
  send_to_console_username();
  if(read_from_console() == CONSOLE_RX_TIMEOUT){
	  oled_display_username_timeout();
	  printf("Err: wait username input timeout\r\n");
	  goto end;
  }
  printf("check username prompt\r\n");
  if(check_passwd_prompt() != 0){
	  oled_display_username_wrong();
	  printf("Err: username prompt wrong\r\n");
	  goto end;
  }
  printf("passwd prompt OK\r\n");

  // check password
  printf("send passwd\r\n");
  oled_display_passwd();
  send_to_console_passwd();
  if(read_from_console() == CONSOLE_RX_TIMEOUT){
	  oled_display_passwd_timeout();
	  printf("Err: wait passwd resp timeout\r\n");
	  goto end;
  }
  printf("check cmd line prompt\r\n");
  if(check_cmdline_prompt() != 0){
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

  while(1){
	  at32_led_toggle(LED2);
	  delay_ms(g_speed * DELAY);
  }

  */

//  while(1)
//  {
//    at32_led_toggle(LED2);
//    delay_ms(g_speed * DELAY);
//    at32_led_toggle(LED3);
//    delay_ms(g_speed * DELAY);
//    at32_led_toggle(LED4);
//    delay_ms(g_speed * DELAY);
//    if(uart2_get_rx_flag() == 1){
//
//    	for(int i =0; i< uart2_get_rx_len(); i++){
//    		printf("%c", buf[i]);
//    	}
//
//    	uart2_clear_rx_flag();
//    	// printf("rx sth.\r\n");
//
//    	uart2_tx_printf("%s\n", "pi");
//
//    	if(uart2_get_rx_flag() == 1){
//
//        	for(int i =0; i< uart2_get_rx_len(); i++){
//        		printf("%c", buf[i]);
//        	}
//        	uart2_clear_rx_flag();
//    	}
//
//    }
//	sprintf(tx_buf, "%s","higogo");
//	uart2_tx_send(tx_buf, strlen(tx_buf));
// uart2_tx_printf("begin...end\n");

//  }
}

/**
  * @}
  */

/**
  * @}
  */
