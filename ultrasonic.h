/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:38
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL    // CPU clock 설정은 무조건 맨 앞에 할 것 (중요)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  // sei 등 함수
#include <stdio.h>

// 초음파 TRIG
#define TRIG_DDR   DDRA
#define TRIG_PORT  PORTA
#define TRIG1_PIN  0
#define TRIG2_PIN  1
#define TRIG3_PIN  2

// 초음파 ECHO
#define ECHO_DDR   DDRE
#define ECHO_PORT  PINE     // external INT 4
#define ECHO1_PIN   4
#define ECHO2_PIN   5
#define ECHO3_PIN   6
