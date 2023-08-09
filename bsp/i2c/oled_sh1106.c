/*
 * oled_sh1106.c
 *
 *  Created on: 2023 Aug 8
 *      Author: ruff
 */

#include "at32f415_board.h"

uint8_t buf[128] = {0};
uint8_t buf_zero[OLED_SH1102_WIDTH] = {0};
uint8_t buf_one[OLED_SH1102_WIDTH] = {0xFF};

void oled_write_cmd(uint8_t reg, uint8_t* tx_buf, uint8_t tx_len){
	i2c_write_cmd(I2C1_OLED_ADDRESS, reg, tx_buf,  tx_len);
}
void oled_write_cmd_byte(uint8_t b){
	buf[0] = b;
	oled_write_cmd(0x0, buf, 1);
}
void oled_write_cmd_bytes(uint8_t *buffer, uint8_t len){
	oled_write_cmd(0x00, buffer, len);
}
void oled_write_data_byte(uint8_t b){
	buf[0] = b;
	oled_write_cmd(0x40, buf, 1);
}
void oled_write_data_bytes(uint8_t *buffer, uint8_t len){
	oled_write_cmd(0x40, buffer, len);
}
void oled_init(){
	oled_write_cmd_byte(0xAE);  // LED off
	oled_write_cmd_byte(0xD5);
	oled_write_cmd_byte(0x80);
	oled_write_cmd_byte(0xA8);
	oled_write_cmd_byte(0x3F);
	oled_write_cmd_byte(0xD3);
	oled_write_cmd_byte(0x00);

	oled_write_cmd_byte(0x40);
	oled_write_cmd_byte(0x8D);

	// internal VCC
	oled_write_cmd_byte(0x14);

	oled_write_cmd_byte(0x20); // Memory Mode
	oled_write_cmd_byte(0x00);
	oled_write_cmd_byte(0xA1); // SEGREMAP
	oled_write_cmd_byte(0xC8); // COM SCAN DEC

	oled_write_cmd_byte(0xDA);
	oled_write_cmd_byte(0x12);
	oled_write_cmd_byte(0x81); // Contrast
	// internal VCC
	oled_write_cmd_byte(0xCF);

	oled_write_cmd_byte(0xD9); // SET Precharge

	// internal VCC
	oled_write_cmd_byte(0xF1);

	oled_write_cmd_byte(0xDB);
	oled_write_cmd_byte(0x40);

	oled_write_cmd_byte(0xA4);
	oled_write_cmd_byte(0xA6);

	// Turn ON
	oled_write_cmd_byte(0xAF);
}
void oled_clear(){
	for(int i = 0; i< OLED_SH1102_WIDTH; i++){
		buf[i] = 0x00;
	}
	for(uint8_t i=0xB0; i<=0xB7; i++){
		oled_write_cmd_byte(i);
		oled_write_cmd_byte(0x0);   // column low addr
		oled_write_cmd_byte(0x10);  // column high addr
//		for(int j=0; j<2; j++){
//			oled_write_data_bytes(buf_zero, 66);
//		}
		oled_write_data_bytes(buf, 132);
	}
}
void oled_all_on(){
	for(int i = 0; i< OLED_SH1102_WIDTH; i++){
		buf[i] = 0xFF;
	}
	for(uint8_t i=0xB0; i<=0xB7; i++){
		oled_write_cmd_byte(i);
		oled_write_cmd_byte(0x0);   // column low addr
		oled_write_cmd_byte(0x10);  // column high addr
//		for(int j=0; j<32; j++){
//			oled_write_data_bytes(buf, 4);
//		}
		oled_write_data_bytes(buf, 128);
	}
}
