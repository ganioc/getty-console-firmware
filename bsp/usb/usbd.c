/*
 * usbd.c
 *
 *  Created on: 2023 Aug 22
 *      Author: ruff
 */

#include "at32f415_board.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"

/** @addtogroup 415_USB_device_vcp_loopback USB_device_vcp_loopback
 * @{
 */

/* usb global struct define */
otg_core_type otg_core_struct;
uint8_t usb_buffer[256];

/**
 * @brief  usb delay millisecond function.
 * @param  ms: number of millisecond delay
 * @retval none
 */
void usb_delay_ms(uint32_t ms) {
	/* user can define self delay function */
	delay_ms(ms);
}

/**
 * @brief  usb delay microsecond function.
 * @param  us: number of microsecond delay
 * @retval none
 */
void usb_delay_us(uint32_t us) {
	delay_us(us);
}

/**
 * @brief  this function handles otgfs interrupt.
 * @param  none
 * @retval none
 */
void OTG_IRQ_HANDLER(void) {
	usbd_irq_handler(&otg_core_struct);
}

#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

#endif

/**
 * @brief  this function config gpio.
 * @param  none
 * @retval none
 */
void usb_gpio_config(void) {
	gpio_init_type gpio_init_struct;

	crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
	gpio_default_para_init(&gpio_init_struct);

	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
#endif

	/* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
	gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
	gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif
}
/**
 * @brief  usb 48M clock select
 * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
 * @retval none
 */
void usb_clock48m_select(usb_clk48_s clk_s) {
	switch (system_core_clock) {
	/* 48MHz */
	case 48000000:
		crm_usb_clock_div_set(CRM_USB_DIV_1);
		break;

		/* 72MHz */
	case 72000000:
		crm_usb_clock_div_set(CRM_USB_DIV_1_5);
		break;

		/* 96MHz */
	case 96000000:
		crm_usb_clock_div_set(CRM_USB_DIV_2);
		break;

		/* 120MHz */
	case 120000000:
		crm_usb_clock_div_set(CRM_USB_DIV_2_5);
		break;

		/* 144MHz */
	case 144000000:
		crm_usb_clock_div_set(CRM_USB_DIV_3);
		break;

	default:
		break;
	}
}

