/*
 * oled_sh1106.c
 *
 *  Created on: 2023 Aug 8
 *      Author: ruff
 */
#include <string.h>

#include "at32f415_board.h"
#include "SH1106Fonts.h"



uint8_t buf[128] = {0};

uint8_t display_buffer[OLED_SH1102_SCREEN_WIDTH * OLED_SH1102_SCREEN_HEIGHT /8];
//uint8_t buf_zero[OLED_SH1102_WIDTH] = {0};
//uint8_t buf_one[OLED_SH1102_WIDTH] = {0xFF};

uint8_t oled_color = WHITE;
uint8_t lastChar;
char *myFontData = (char *)ArialMT_Plain_16;
int myTextAlignment = TEXT_ALIGN_LEFT;

const char *p_str = "Get IP Address: 192.168.0.99 ";
const char *p_welcome = "Getty Console v1.0 ";
const char *p_login_timeout = "Err: login timeout ";
const char *p_login = "Login ... ";

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

void oled_all_clear(){
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
//		for (int j = 0; j< 11; j++){
//			// oled_write_data_bytes(buf, 132);
//			oled_write_data_bytes(buf, 12);
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
//		for(int j = 0; j< 128; j++){
//			// ;
//			// oled_write_data_bytes(buf, 8);
//			// oled_write_data_byte(0xFF);
//		}
		oled_write_data_bytes(buf, 128);

	}
}
void oled_display_off(){
	oled_write_cmd_byte(0xAE);  // display off
}
void oled_display_on(){
	oled_write_cmd_byte(0xAF);
}
void oled_clear_buffer(void){
	memset(display_buffer, 0, (OLED_SH1102_SCREEN_WIDTH * OLED_SH1102_SCREEN_HEIGHT / 8));
}
void oled_display_from_buffer(void){
	/*
	oled_write_cmd_byte(COLUMNADDR);
	oled_write_cmd_byte(0x00);
	oled_write_cmd_byte(0x7F);

	oled_write_cmd_byte(PAGEADDR);
	oled_write_cmd_byte(0x00);
	oled_write_cmd_byte(0x7);

	oled_write_cmd_byte(SETSTARTLINE | 0x00);

	for(uint16_t i=0; i< OLED_SH1102_BYTES; i+=16){

		oled_write_data_bytes(&display_buffer[i], 16);
	}*/
	for(uint8_t i=0; i<=7; i++){

		oled_write_cmd_byte(0xB0 + i);
		oled_write_cmd_byte(0x02);   // column low addr
		oled_write_cmd_byte(0x10);  // column high addr

		oled_write_data_bytes(&display_buffer[i*128], 128);
	}

}
void oled_reset_display(void){
	oled_display_off();
	oled_clear_buffer();
	oled_display_from_buffer();
	oled_display_on();
}
void oled_set_pixel(uint8_t x, uint8_t y){
	if(x >= 0 && x < OLED_SH1102_SCREEN_WIDTH && y >=0 && y < OLED_SH1102_SCREEN_HEIGHT){
		switch(oled_color){
		case WHITE:   display_buffer[x + (y/8)*128] |= (1 << (y&7)); break;
	    case BLACK:   display_buffer[x + (y/8)*128] &= ~(1 << (y&7)); break;
	    case INVERSE: display_buffer[x + (y/8)*128] ^=  (1 << (y&7)); break;
		}
	}
}
void oled_set_char(uint8_t x, uint8_t y, unsigned char data){
	for(int i =0; i< 8; i++){
		if((data & (1<<i)) != 0 ){
			oled_set_pixel(x, y + i);
		}
	}
}
uint8_t oled_utf8ascii(uint8_t ascii){
	if(ascii < 128){
		lastChar = 0;
		return (ascii);
	}

	// get previous input
	uint8_t last = lastChar; // get last char
	lastChar = ascii;        // remember actual character

	switch(last){ // conversion depending on first UTF8-character
	case 0xC2:
		return (ascii);
		break;
	case 0xC3:
		return (ascii | 0xC0);
		break;
	case 0x82:
		if(ascii == 0xAC){
			return (0x80); // special case Euro-symbol
		}
	}
	return 0; // otherwise return zero, if character has to be ignored
}
void oled_set_text_alignment(int textAlignment){
	myTextAlignment = textAlignment;
}
void oled_set_font(int num){
	switch(num){
	case 10:
		myFontData = (char *)ArialMT_Plain_10;
		break;
	case 16:
		myFontData = (char *)ArialMT_Plain_16;
		break;
	case 24:
		myFontData = (char *)ArialMT_Plain_24;
		break;
	default:
		printf("Wrong font num!\n");
		break;
	}

}
int oled_get_string_width(uint8_t* buf, uint8_t len){
	int stringWidth = 0;
	uint8_t charCode;
	for(int j = 0; j< len; j++){
		charCode = buf[j] - 0x20;
		stringWidth += myFontData[CHAR_WIDTH_START_POS + charCode];
	}
	return stringWidth;
}
void oled_draw_string(uint8_t x, uint8_t y, uint8_t* buf, uint8_t len){
	unsigned char currentByte;
	int charX, charY;
	int currentBitCount;
	int charCode;
	int currentCharWidth;
	int currentCharStartPos;
	int cursorX = 0;


	int numberOfChars = myFontData[CHAR_NUM_POS];
	// iterate over string
	int firstChar = myFontData[FIRST_CHAR_POS];
	int charHeight = myFontData[HEIGHT_POS];
	int charWidth = myFontData[WIDTH_POS];
	int currentCharByteNum = 0;
	int startX = 0;
	int startY = y;

	int width = 0;

	if(myTextAlignment == TEXT_ALIGN_LEFT){
		startX = x;
	}else if(myTextAlignment == TEXT_ALIGN_CENTER){
		width = oled_get_string_width(buf, len);
		startX = x - width/2;
	}else if(myTextAlignment == TEXT_ALIGN_RIGHT){
		width = oled_get_string_width(buf, len);
		startX = x - width;
	}

	for(int j = 0; j < len; j++){
		charCode = buf[j] - 0x20; // 0x20 1st character, SP,
		currentCharWidth = myFontData[CHAR_WIDTH_START_POS + charCode];
		// Jump to font data beginning
		currentCharStartPos = CHAR_WIDTH_START_POS + numberOfChars;

		for(int m = 0; m < charCode; m++){
			currentCharStartPos += myFontData[CHAR_WIDTH_START_POS + m ] * charHeight /8 + 1;
		}
		currentCharByteNum = ((charHeight * currentCharWidth) / 8) + 1;
		// iterate over all bytes of character
		for(int i = 0; i < currentCharByteNum; i++){
			currentByte = myFontData[currentCharStartPos + i];

			// iterate over all bytes of character
			for(int bit = 0; bit < 8; bit++){
				currentBitCount = i * 8 + bit;

				charX = currentBitCount % currentCharWidth;
				charY = currentBitCount / currentCharWidth;

				if( (currentByte & (1 << bit)) != 0){
					oled_set_pixel(startX + cursorX + charX, startY + charY);
				}
			}
		}
		cursorX += currentCharWidth;

	}
}

void oled_draw_string_max_width(int x, int y, int maxLineWidth, uint8_t * buf, uint8_t len){
	int currentLineWidth = 0;
	int startsAt = 0;
	int endsAt = 0;
	int lineNumber = 0;
	char currentChar = ' ';
	int lineHeight = myFontData[HEIGHT_POS];
	int lineCandidate = 0;

	for(int i = 0; i < len; i++){
		currentChar = buf[i];
		if( currentChar == ' ' || currentChar == '-'){
			lineCandidate = i;
			if(oled_get_string_width(buf + startsAt, lineCandidate - startsAt) <= maxLineWidth){
				endsAt = i;
			}else{
				oled_draw_string(x, y + lineNumber * lineHeight , buf + startsAt, endsAt - startsAt);
				lineNumber++;
				startsAt = endsAt + 1;
			}
		}
	}
	oled_draw_string(x, y + lineNumber * lineHeight, buf + startsAt, len - startsAt);
}
void oled_buffer_clear(void){
	for(int i = 0; i< OLED_SH1102_SCREEN_WIDTH * OLED_SH1102_SCREEN_HEIGHT /8; i++){
		display_buffer[i] = 0x0;
	}
}

void oled_display_welcome(){
	oled_buffer_clear();
	oled_draw_string_max_width(0,0,128,(uint8_t *)p_welcome,strlen(p_welcome));
	oled_display_from_buffer();
}
void oled_display_login(){
	oled_buffer_clear();
	oled_draw_string_max_width(0,0,128,(uint8_t *)p_login,strlen(p_login));
	oled_display_from_buffer();
}
void oled_display_login_timeout(){
	oled_buffer_clear();
	oled_draw_string_max_width(0,0,128,(uint8_t *)p_login_timeout,strlen(p_login_timeout));
	oled_display_from_buffer();
}
