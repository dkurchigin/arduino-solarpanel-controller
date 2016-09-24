#include <VirtualWire.h>

#define LED_PIN 13
#define ENA 9
#define ENB 10
#define IN4 8
#define IN3 7
#define IN2 6
#define IN1 5

#define STOP 0
#define HALF 150
#define FULL 255 

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN1, OUTPUT);
  vw_set_ptt_inverted(true); // необходимо для модуля DR3100
  vw_set_rx_pin(12);
  vw_setup(4000); // бит в секунду
  vw_rx_start(); // запуск фазовой автоподстройки частоты (ФАПЧ)
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) {  // Не блокируется
    if (buf[0] == 'c' && buf[1] == 'm') {
      digitalWrite(LED_PIN,HIGH);
      Serial.println("got command");
      if (buf[2] == '4') {
        if (buf[3] == '1') {
          Serial.println("Move Forward");
          motor_state(1, HALF, HALF);  
        } else if (buf[3] == '2') {
          Serial.println("Move Back");
          motor_state(2, HALF, HALF);
        } else if (buf[3] == '3') {
          Serial.println("Move Left");
          motor_state(1, HALF, STOP);
        } else if (buf[3] == '4') {
          Serial.println("Move Right");
          motor_state(1, STOP, HALF);
        }       
      }
    }
    if (buf[0]=='0') {
      digitalWrite(LED_PIN,LOW);
      Serial.println("have not command");
      motor_state(0, STOP, STOP); 
    }
  }
  digitalWrite(LED_PIN,LOW);
  Serial.println("have not nothing");
  motor_state(0, STOP, STOP);
}

void motor_state(int state, int rightMotor, int leftMotor) {
  int in1, in2, in3, in4;
  if (state == 0) {
    in1 = in2 = in3 = in4 = 0;
  } else if (state == 1) {
    in1 = in4 = 255;
    in2 = in3 = 0;
  } else {
    in1 = in4 = 0;
    in2 = in3 = 255;
  }
  
  digitalWrite(IN1, in1);
  digitalWrite(IN2, in2);
  digitalWrite(IN3, in3);
  digitalWrite(IN4, in4);
  analogWrite(ENA, rightMotor);
  analogWrite(ENB, leftMotor);
}
