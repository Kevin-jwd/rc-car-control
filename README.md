# ATmega128A 기반 자율주행 RC Car

**기간:** YYYY.MM ~ YYYY.MM · **참여 인원:** 1명(개별 차량) · **구분:** 개인 구현·공동 발표

## 개요

초음파 센서 3개의 거리 데이터를 이용한 장애물 대응 주행과 Bluetooth 명령 기반 수동 주행을 지원하는 RC Car입니다.
ATmega128A에서 센서 수집·필터링·주행 판단·모터 출력·FND 표시를 처리합니다.
각자 차량을 구현하고 발표를 함께 진행한 프로젝트입니다.

| 구분 | 구성 |
| --- | --- |
| MCU·개발 환경 | ATmega128A, C, Microchip Studio |
| 거리 센서 | HC-SR04 초음파 센서 3개 |
| 무선 입력 | ZS-040 Bluetooth 보드, UART1 |
| 구동 | L298N 모터 드라이버, DC 모터, Timer1 PWM·GPIO |
| 표시·입력 | FND 2개, 버튼, 상태 LED |

## 담당

- 개별 차량의 센서 수집과 거리 데이터 처리
- 자율주행 판단 및 Bluetooth 수동 제어
- PWM 모터 제어와 주행 상태·조회 정보 표시
- 버튼 입력을 통한 주행·조회 모드 전환

## 구현

| 파일 | 내용 |
| --- | --- |
| `main.c` | 초기화, 함수 포인터 기반 6개 모드, 수동·자율주행 판단 |
| `ultrasonic.c`, `ultrasonic.h` | 외부 인터럽트·Timer3 기반 Echo 측정, 센서별 칼만 필터 |
| `pwm.c`, `pwm.h` | Timer1 PWM과 GPIO 방향 신호를 이용한 모터 제어 |
| `uart1.c`, `uart1.h` | Bluetooth 수동 명령 수신 |
| `uart0.c`, `uart0.h` | PC 통신·디버그 출력 |
| `fnd.c`, `fnd.h` | 주행 상태·시간·통계 조회 화면 |
| `button.c`, `button.h` | 모드 전환 버튼 입력·디바운스 |
| `led.c`, `led.h`, `def.h` | 보조 출력과 상태 상수 |
| `07.AUTO_CAR.cproj` | Microchip Studio 프로젝트 설정 |

Microchip Studio에서 `07.AUTO_CAR.cproj`를 열어 프로젝트를 빌드합니다.
이 저장소는 로컬 보관 소스를 내용 변경 없이 분리한 것으로, 현재 환경에서 AVR 재빌드와 실제 차량 구동은 검증하지 않았습니다.
시간 환산·통계 집계·제어 분기·공유 GPIO에 관한 기존 정적 검토 내용은 관련 문서에 기록했습니다.

## 관련 문서

- [프로젝트 상세 문서](https://kevin-jwd.github.io/projects/rc-car.html)
- [코드 확인 결과·개선 과제](docs/code-review.md)
- [소스 분리 기록](SOURCE.md)
