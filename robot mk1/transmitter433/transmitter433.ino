#include <VirtualWire.h>

#define LED_PIN 13

#define MOVE_FORWARD "cm41"
#define MOVE_BACK "cm42"
#define MOVE_LEFT "cm43"
#define MOVE_RIGHT "cm44"

char *cont;
int message;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN,OUTPUT);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(12);
  vw_setup(4000);// скорость передачи данных в Kbps
}

void loop() {
  
  //send_message(MOVE_FORWARD);
  //delay(1000);
  //send_message(MOVE_BACK);
  //delay(1000);
  digitalWrite(LED_PIN, LOW);
  if (Serial.available() > 0) {
    message = Serial.read();
    if (message == '1') {
      send_message(MOVE_FORWARD);
      //cont = MOVE_FORWARD;
      //vw_send((uint8_t *)cont, strlen(cont));
      //vw_wait_tx();
    } else if (message == '2') {
      send_message(MOVE_BACK);
    } else if (message == '3') {
      send_message(MOVE_LEFT);
    } else if (message == '4') {
      send_message(MOVE_RIGHT);
    }
  }
  /*cont = MOVE_FORWARD;
  vw_send((uint8_t *)cont, strlen(cont));
  vw_wait_tx();
  delay(2000);
  cont = MOVE_BACK;
  vw_send((uint8_t *)cont, strlen(cont));
  vw_wait_tx();
  delay(2000);*/
}

void send_message(char *command) {
  char *message = command;
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx();
  Serial.println(command);
  digitalWrite(LED_PIN, HIGH);
}

