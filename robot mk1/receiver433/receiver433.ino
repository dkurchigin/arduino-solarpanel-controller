#include <VirtualWire.h>

#define LED_PIN 13
#define ENA A0
#define ENB A1
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
    Serial.println(buf[0]);
    if (buf[0] == 'c' && buf[1] == 'm') {
      //digitalWrite(LED_PIN,HIGH);
      //Serial.println("got command");
      if (buf[2] == '4') {
        if (buf[3] == '1') {
          digitalWrite(LED_PIN,HIGH);
          Serial.println("Move Forward");
          motor_state(1, HALF, HALF);
          delay(100);  
        } else if (buf[3] == '2') {
          digitalWrite(LED_PIN,HIGH);
          Serial.println("Move Back");
          motor_state(2, HALF, HALF);
          delay(100);
        } else if (buf[3] == '3') {
          digitalWrite(LED_PIN,HIGH);
          Serial.println("Move Left");
          motor_state(1, HALF, STOP);
          delay(100);
        } else if (buf[3] == '4') {
          digitalWrite(LED_PIN,HIGH);
          Serial.println("Move Right");
          motor_state(1, STOP, HALF);
          delay(100);
        }       
      }
    }
    if (buf[0]=='0') {
      digitalWrite(LED_PIN,LOW);
      //Serial.println("have not command");
      motor_state(0, STOP, STOP); 
    }
  }
  /*digitalWrite(LED_PIN, HIGH);
  //Serial.println("have not nothing");
  motor_state(0, STOP, STOP);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  //Serial.println("have not nothing");
  motor_state(1, HALF, HALF);
  delay(1000);*/
}

void motor_state(int state, int rightMotor, int leftMotor) {
  if (state == 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  } else if (state == 1) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  analogWrite(ENA, rightMotor);
  analogWrite(ENB, leftMotor);
}
