/*
 * fnd.c
 *
 * Created: 2026-06-22 오후 2:36:02
 *  Author: kccistc
 */ 
#include "fnd.h"

void init_fnd(void);
void turn_off_fnd(void);

void stop_display(void);
void forward_display(void);
void turn_left_display(void);
void turn_right_display(void);
void backward_display(void);
void backward_cnt_display(void);
void turn_left_cnt_display(void);
void turn_right_cnt_display(void);

extern volatile uint32_t ms_count;
extern volatile uint32_t sec_count;
extern volatile uint32_t min_count;
extern volatile uint32_t display_sec_count = 0;

extern volatile int distance[3];

extern int lap_time;
extern int backward_cnt;
extern int turn_left_cnt;
extern int turn_right_cnt;


int dot_bit = 0x80;
int animation_bit = 0xff;

void init_fnd(void)
{
	FND_DATA_DDR = 0xFF;          // 출력 모드로 설정
	FND1_CONTROL_DDR |= 1 << FND1_CONTROL_D1 | 1 << FND1_CONTROL_D2 | 1 << FND1_CONTROL_D3 | 1 << FND1_CONTROL_D4;
	FND2_CONTROL_DDR |= 1 << FND2_CONTROL_D1 | 1 << FND2_CONTROL_D2 | 1 << FND2_CONTROL_D3 | 1 << FND2_CONTROL_D4;
	turn_off_fnd();
}

void turn_off_fnd(void)
{
	FND_DATA_PORT = 0x00;
}

void toggle_dot(void)
{
	dot_bit = (dot_bit == 0x80) ? 0x00 : 0x80;
}

void toggle_animation(void)
{
	animation_bit = (animation_bit == 0x00) ? 0xff : 0x00;
}

// ======================================
// ---- 좌측 FND 디스플레이 (주행시간) ----
// ---- 우측 FND 디스플레이 (속력) ----
// ======================================

// ---- stop 디스플레이 ----
void stop_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;
	
	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;
	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	switch(digit_select)
	{
		// 좌측
		case 0:
			FND1_CONTROL_PORT = ~0x80;
			FND_DATA_PORT = left_animation_display;
			break;
		case 1:
			FND1_CONTROL_PORT = ~0x40;
			FND_DATA_PORT = left_animation_display;
			break;
		case 2:
			FND1_CONTROL_PORT = ~0x20;
			FND_DATA_PORT = left_animation_display;
			break;
		case 3:
			FND1_CONTROL_PORT = ~0x10;
			FND_DATA_PORT = left_animation_display;
			break;
		
		// 우측
		case 4:
			FND2_CONTROL_PORT = ~0x80;
			FND_DATA_PORT = right_animation_display;
			break;
		case 5:
			FND2_CONTROL_PORT = ~0x40;
			FND_DATA_PORT = right_animation_display;
			break;
		case 6:
			FND2_CONTROL_PORT = ~0x20;
			FND_DATA_PORT = right_animation_display;
			break;
		case 7:
			FND2_CONTROL_PORT = ~0x10;
			FND_DATA_PORT = right_animation_display;
			break;
	}
	digit_select = (digit_select + 1) % 8;
}

// ---- forward 디스플레이 ----
void forward_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;
	dot_bit = 0x80;
	
	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;

	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = numbers[min_count % 10];
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = numbers[min_count/10 % 6];
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = left_animation_display;
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = left_animation_display;
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = right_animation_display;
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = right_animation_display;
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[sec_count % 10];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[sec_count/10 % 6];
		break;
	}
	digit_select = (digit_select + 1) % 8;
}

// ---- turn_left 디스플레이 ----
void turn_left_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;
	static int dot_count = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;

	dot_count++;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = numbers[sec_count % 10];
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = numbers[sec_count/10 % 6];
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[min_count % 10];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[min_count/10 % 6];
		break;
	}
	digit_select = (digit_select + 1) % 8;
	if(dot_count >= 200)
	{
		toggle_animation();
		dot_count = 0;
	}
}

// ---- turn_right 디스플레이 ----
void turn_right_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;
	static int dot_count = 0;

	if (ms_count - prev_ms < 1)
	{
		return; 
	}
	prev_ms = ms_count; 

	dot_count++;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = numbers[sec_count % 10];
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = numbers[sec_count/10 % 6];
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[min_count % 10];
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[min_count/10 % 6];
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
	}
	digit_select = (digit_select + 1) % 8;
		if(dot_count >= 200)
		{
			toggle_animation();
			dot_count = 0;
		}
}

// ---- backward 디스플레이 ----
void backward_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;
	static int dot_count = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;

	dot_count++;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = left_animation_display & animation_bit;
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = right_animation_display & animation_bit;
		break;
	}
	digit_select = (digit_select + 1) % 8;
	if(dot_count >= 200)
	{
		toggle_animation();
		dot_count = 0;
	}
}

// ---- 주행 시간 디스플레이 ----
void lap_time_display()
{	
	static uint32_t prev_ms = 0;
	static int digit_select = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;


	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = numbers[lap_time % 10];
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = numbers[lap_time/10 % 6];
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = lap_time_display2[display_sec_count % 8];
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = lap_time_display1[display_sec_count % 8];
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = lap_time_display3[display_sec_count % 8];
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = lap_time_display4[display_sec_count % 8];
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[lap_time/60 % 10];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[lap_time/60/10];
		break;
	}
	digit_select = (digit_select + 1) % 8;
}

// ---- 후진 횟수 디스플레이 ----
void backward_cnt_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = 0x77;
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = 0x7F;
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = backward_animation_display[display_sec_count % 6];
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = backward_animation_display[display_sec_count % 6];
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = backward_animation_display[display_sec_count % 6];
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = backward_animation_display[display_sec_count % 6];
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[backward_cnt / 20 % 100];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[(int)(backward_cnt / 20 % 10)];
		break;
	}
	digit_select = (digit_select + 1) % 8;
}

// ---- 좌회전 횟수 디스플레이 ----
void turn_left_cnt_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;


	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = 0x79;
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = 0x38;
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = lap_time_display2[display_sec_count % 8];
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = lap_time_display1[display_sec_count % 8];
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = lap_time_display2[display_sec_count % 8];
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = lap_time_display1[display_sec_count % 8];
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[turn_left_cnt / 20 % 100];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[(int)(turn_left_cnt / 20 % 10)];
		break;
	}
	digit_select = (digit_select + 1) % 8;
}

// ---- 우회전 횟수 디스플레이 ----
void turn_right_cnt_display()
{
	static uint32_t prev_ms = 0;
	static int digit_select = 0;

	if (ms_count - prev_ms < 2)
	{
		return;
	}
	prev_ms = ms_count;

	FND1_CONTROL_PORT = 0xFF;
	FND2_CONTROL_PORT = 0xFF;
	
	switch(digit_select)
	{
		// 좌측
		case 0:
		FND1_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = 0x30;
		break;
		case 1:
		FND1_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = 0xF7;
		break;
		case 2:
		FND1_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = lap_time_display3[display_sec_count % 8];
		break;
		case 3:
		FND1_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = lap_time_display4[display_sec_count % 8];
		break;
		
		// 우측
		case 4:
		FND2_CONTROL_PORT = ~0x80;
		FND_DATA_PORT = lap_time_display3[display_sec_count % 8];
		break;
		case 5:
		FND2_CONTROL_PORT = ~0x40;
		FND_DATA_PORT = lap_time_display4[display_sec_count % 8];
		break;
		case 6:
		FND2_CONTROL_PORT = ~0x20;
		FND_DATA_PORT = numbers[(int)(turn_right_cnt / 20 % 100)];
		break;
		case 7:
		FND2_CONTROL_PORT = ~0x10;
		FND_DATA_PORT = numbers[(int)(turn_right_cnt / 20 % 10)];
		break;
	}
	digit_select = (digit_select + 1) % 8;
}