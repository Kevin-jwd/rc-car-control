// 오픈소스 방식: 함수 이름은 소문자, 의미가 바뀌는 순간 '_' 추가
/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"
void init_led();
void turn_off_led();
void turn_on_led();

void init_led()
{
	DDRG = 0xff;
	PORTG = 0x00;
}

void turn_off_led()
{
	PORTG = 0x00;
}

void turn_on_led()
{
	PORTG = 0x08;
}