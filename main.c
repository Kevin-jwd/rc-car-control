/*
 * 06.DCMOTOR_PWM_CONTROL.c
 *
 * Created: 2026-06-15 오전 11:36:41
 * Author : kccistc
 */ 

#define F_CPU 16000000UL   // CPU clock 설정은 무조건 맨 앞에 할 것 (중요) 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  // sei 등 함수 
#include <stdio.h>

#include "def.h"
#include "button.h"

extern void init_led();
extern void turn_on_led();

extern void init_timer0(void);
extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void pc_command_processing(void);
extern void init_ultrasonic(void);
extern void process_ultrasonic(void);

extern void init_timer1_pwm(void);
extern void init_motor_driver(void);
extern void init_button(void);
extern void init_uart1(void);
extern volatile uint8_t bt_data;
extern void forward(int speed);
extern void backward(int speed);
extern void turn_left(int speed);
extern void turn_right(int speed);
extern void stop();

extern void init_fnd(void);
extern void stop_display(void);
extern void forward_display(void);
extern void turn_left_display(void);
extern void turn_right_display(void);
extern void backward_display(void);
extern void lap_time_display(void);
extern void backward_cnt_display(void);
extern void turn_left_cnt_display(void);
extern void turn_right_cnt_display(void);

void manual_mode(void);
void auto_mode(void);
void distance_check(void);
void lap_time_check(void);
void backward_time_check(void);
void turn_left_time_check(void);
void turn_right_time_check(void);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

typedef enum STATE { LEFT_OBSTACLE, RIGHT_OBSTACLE, FRONT_OBSTACLE, NO_OBSTACLE };

volatile int ultrasonic_check_time= 0;
extern volatile int distance[3];

volatile uint32_t ms_count = 0;
volatile uint32_t sec_count = 0;
volatile uint32_t min_count = 0;
volatile uint32_t display_ms_count = 0;
volatile uint32_t display_sec_count;

int func_state=MANUAL_MODE;
int lap_time = 0;
int is_finished = 0;
int current_state = 0;

int backward_cnt = 0;
int turn_left_cnt = 0;
int turn_right_cnt = 0;

ISR(TIMER0_OVF_vect)            // 벡터 번호 17
{
	TCNT0 = 6;
	ms_count++;
	ultrasonic_check_time++;
	display_ms_count++;
	
	if(ms_count >= 1000)
	{
		ms_count = 0;
	    sec_count++;
	}
	if(sec_count >= 60)
	{
		sec_count = 0;
		min_count++;
	}
	if(display_ms_count >= 100)
	{
		display_ms_count = 0;
		display_sec_count++;
	}
}

void (*pfunc[]) () = 
{
	manual_mode,
	auto_mode,
	lap_time_check,
	backward_time_check,
	turn_left_time_check,
	turn_right_time_check
};

int main(void)
{
    init_led();
	init_timer0();
	init_uart0();
	init_uart1();
	init_fnd();
	init_button();
	init_motor_driver();
	init_timer1_pwm();
	init_ultrasonic();
		
	stdout = &OUTPUT;  // printf가 동작 할 수 있도록 stdout을 설정
	sei();	// 전역(대문) interrupt 허용 
	
	// led_main();
	while (1) 
    {
		process_ultrasonic();
		pfunc[func_state]();
		if(get_button(BUTTON0, BUTTON0PIN))
		{
			func_state = (func_state + 1) % 6;
		}
    }
}

void init_var()
{
	min_count = 0;
	sec_count = 0;	
	lap_time = 0;
	is_finished = 0;
	backward_cnt = 0;
	turn_left_cnt = 0;
	turn_right_cnt = 0;
}


void manual_mode(void)
{
	init_var();
	
	switch(bt_data) {
		case 'F':
		case 'f':
			forward(500);    // 4us x 500 = 0.002sec(2ms)
			forward_display();
			break;
		case 'B':
		case 'b':
			backward(500);
			backward_display();
			break;
		case 'L':
		case 'l':
			turn_left(700);	 // 4us x 700 = 0.0028sec 
			turn_left_display();
			break;
		case 'R':
		case 'r':
			turn_right(700);
			turn_right_display();
			break;
		case 'S':
		case 's':
			stop();
			stop_display();
			break;
		default:
			stop();
			stop_display();
			break;
	}
}

void auto_mode(void)
{		
	turn_on_led();

	static int prev_distance[3] = {0, 0, 0};
		
	// 현재 거리와 이전 거리의 차이
	int diff_left  = distance[0] - prev_distance[0];
	int diff_front = distance[1] - prev_distance[1];
	int diff_right = distance[2] - prev_distance[2];

	prev_distance[0] = distance[0];
	prev_distance[1] = distance[1];
	prev_distance[2] = distance[2];

	//distance[1] = (distance[1] >= 80) ? distance[1] / 10 : distance[1];
	//distance[2] = (distance[2] >= 80) ? distance[2] / 10 : distance[2];

	if(distance[0] < 10 || distance[1] < 20 || distance[2] < 10)
	{
		backward(400);
		backward_display();
		return;
	}

#if 1
	// 장애물이 다가오는 중이면 감속
	if (distance[1] < 40 || diff_front < 0)
	{
		// 정면에 장애물이 있거나 급격히 다가올 때 양옆 센서 비교
		if (distance[0] > distance[2]) {
			turn_left(400); // 왼쪽 공간이 더 넓으면 왼쪽으로 회전
			turn_left_display();
			turn_left_cnt++;
		} 
		else 
		{
			turn_right(400); // 오른쪽 공간이 더 넓으면 오른쪽으로 회전
			turn_right_display();
			turn_right_cnt++;
		}
	}
	
	// 전방이 안전할 때
	// 왼쪽 벽이 다가오면 우회전, 반대면 좌회전
	int steer_weight = (distance[0] - distance[2]) * 2 + (diff_left - diff_right) * 5;

	if (steer_weight > 10) 
	{
		turn_left(400);  // 오른쪽 벽이 더 가까움
		turn_left_display();
		turn_left_cnt++;
	} 
	else if (steer_weight < -10) 
	{
		turn_right(400); // 왼쪽 벽이 더 가까움
		turn_right_display();
		turn_right_cnt++;
	} 
	else 
	{
		forward(400);    // 밸런스가 맞으면 직진
		forward_display();
	}

#else
	if (distance[0] < 20 || distance[1] < 30 || distance[2] < 20 || diff_front < -2)
	{
		// 정면에 장애물이 있거나 급격히 다가올 때 양옆 센서 비교
		if (distance[0] > distance[2]) {
			turn_left(500); // 왼쪽 공간이 더 넓으면 왼쪽으로 회전
			turn_left_display();
		}
		else
		{
			turn_right(500); // 오른쪽 공간이 더 넓으면 오른쪽으로 회전
			turn_right_display();
		}
	}
	if(distance[0] < 20)
	{
		turn_right(400);
		turn_right_display();
	}
	if(distance[1] < 20)
	{
		turn_left(400);
		turn_left_display();
	}
	
#endif
}

void lap_time_check(void)
{
	stop();
	turn_off_led();
	if(!is_finished)
	{
		lap_time = min_count + 60 * sec_count;
		is_finished = 1;
	}
	lap_time_display();
}

void backward_time_check(void)
{
	backward_cnt_display();
}

void turn_left_time_check(void)
{
	turn_left_cnt_display();
}

void turn_right_time_check(void)
{
	turn_right_cnt_display();
}

/*
ISR (Interrupt Service Routine) : 인터럽트 처리 함수 ISR로 시작
TIMER0_OVF_vect : Timer 0 Overflow INT가 발생이 되면 이곳으로 진입 함
250개의 펄스를 count(1ms) 하면 이곳으로 자동 진입 한다. 
ISR은 가능한 짧게 작성 한다. 
*/

void init_timer0(void)
{
	TCNT0 = 6;   // TCNT0 6~256 : 250개 펄스 count하기 위해 
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주 
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;	   // 64분주  
	TIMSK |= 1 << TOIE0;	// TIMER0 Overflow INT
}


