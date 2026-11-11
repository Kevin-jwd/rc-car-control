#include "ultrasonic.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

extern volatile int ultrasonic_check_time;

volatile int distance[3] = { 0 };       // 필터링된 최종 거리
volatile int raw_distance[3] = { 0 };   // 센서가 읽은 직후의 데이터
volatile uint16_t rising_time[3] = { 0 };

static const uint8_t trig_pins[3] = { TRIG1_PIN, TRIG2_PIN, TRIG3_PIN };

// --- 칼만 필터 변수 및 함수 정의 ---
// Q: 프로세스 노이즈 (작을수록 부드럽고 반응이 느려짐, 클수록 생데이터에 가까워짐)
// R: 측정 노이즈 (초음파 센서 자체의 노이즈 크기, 값이 튈 때 클수록 잘 잡아줌)
static const float Q = 0.5;
static const float R = 5.0;

static float x_est[3] = { 40.0, 40.0, 40.0 }; // 추정 상태 변수 (초기값 40cm 설정)
static float P[3] = { 1.0, 1.0, 1.0 };         // 오차 공분산 초기값

float kalman_filter(int sensor_idx, float measurement)
{
	// 1. 예측 (Prediction)

	P[sensor_idx] = P[sensor_idx] + Q;

	// 2. 보정 (Correction)
	float K = P[sensor_idx] / (P[sensor_idx] + R); // 칼만 이득 (Kalman Gain)
	x_est[sensor_idx] = x_est[sensor_idx] + K * (measurement - x_est[sensor_idx]);
	P[sensor_idx] = (1.0 - K) * P[sensor_idx];

	return x_est[sensor_idx];
}

void init_ultrasonic(void)
{
	TRIG_DDR |= (1 << TRIG1_PIN) | (1 << TRIG2_PIN) | (1 << TRIG3_PIN);
	ECHO_DDR &= ~((1 << ECHO1_PIN) | (1 << ECHO2_PIN) | (1 << ECHO3_PIN));
	
	EICRB |= (1 << ISC40) | (1 << ISC50) | (1 << ISC60);
	EICRB &= ~((1 << ISC41) | (1 << ISC51) | (1 << ISC61));
	
	EIMSK |= (1 << INT4) | (1 << INT5) | (1 << INT6);
	
	TCCR3A = 0x00;
	TCCR3B = 0x00;
	TCCR3B |= (1 << CS32) | (1 << CS30);
	
	ultrasonic_check_time = 60;
}

ISR(INT4_vect)
{
	if(PINE & (1 << ECHO1_PIN))
	{
		rising_time[0] = TCNT3;
	}
	else
	{
		uint16_t dt = TCNT3 - rising_time[0];
		raw_distance[0] = (dt * 64) / 58; // 생데이터 저장
	}
}

ISR(INT5_vect)
{
	if(PINE & (1 << ECHO2_PIN))
	{
		rising_time[1] = TCNT3;
	}
	else
	{
		uint16_t dt = TCNT3 - rising_time[1];
		raw_distance[1] = (dt * 64) / 58;
	}
}

ISR(INT6_vect)
{
	if(PINE & (1 << ECHO3_PIN))
	{
		rising_time[2] = TCNT3;
	}
	else
	{
		uint16_t dt = TCNT3 - rising_time[2];
		raw_distance[2] = (dt * 64) / 58;
	}
}

void make_trigger(int pin_number)
{
	TRIG_PORT &= ~(1 << pin_number);
	_delay_us(2);
	TRIG_PORT |= (1 << pin_number);
	_delay_us(10);
	TRIG_PORT &= ~(1 << pin_number);
}

void process_ultrasonic(void)
{
	static int sensor_select = 0;
	
	if (ultrasonic_check_time >= 60)
	{
		ultrasonic_check_time = 0;
		
		distance[sensor_select] = (int)kalman_filter(sensor_select, (float)raw_distance[sensor_select]);

		sensor_select = (sensor_select + 1) % 3;
		make_trigger(trig_pins[sensor_select]);
	}
}