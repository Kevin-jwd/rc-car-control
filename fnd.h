/*
 * fnd.h
 *
 * Created: 2026-06-22 오후 2:34:03
 *  Author: kccistc
 */ 


#ifndef FND_H_
#define FND_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 공통 DATA 선
#define FND_DATA_DDR DDRC
#define FND_DATA_PORT PORTC

// 좌측 FND
#define FND1_CONTROL_DDR DDRF
#define FND1_CONTROL_PORT PORTF

#define FND1_CONTROL_D1 4
#define FND1_CONTROL_D2 5
#define FND1_CONTROL_D3 6
#define FND1_CONTROL_D4 7

// 우측 FND
#define FND2_CONTROL_DDR DDRA
#define FND2_CONTROL_PORT PORTA

#define FND2_CONTROL_D1 4
#define FND2_CONTROL_D2 5
#define FND2_CONTROL_D3 6
#define FND2_CONTROL_D4 7

uint8_t numbers[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67};
uint8_t lap_time_display1[8] = { 0x01, 0x21, 0x30, 0x18, 0x08, 0, 0, 0 };      
uint8_t lap_time_display2[8] = { 0x01, 0, 0, 0, 0x08, 0x0C, 0x06, 0x03 };	
uint8_t lap_time_display3[8] = { 0x01, 0x03, 0x06, 0x0C, 0x08, 0, 0, 0 };
uint8_t lap_time_display4[8] = { 0x01, 0, 0, 0, 0x08, 0x18, 0x30, 0x21 };

 uint8_t left_animation_display = 0x39;
 uint8_t right_animation_display = 0x0F;

uint8_t backward_animation_display[6] = { 0x00, 0x01, 0x41, 0x49, 0x48, 0x08 };

uint8_t dot_display = 0x80;

#endif /* FND_H_ */