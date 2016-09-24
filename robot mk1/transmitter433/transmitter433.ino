#include <VirtualWire.h>

#define LED_PIN 13

#define MOVE_FORWARD "cm41"
#define MOVE_BACK "cm42"
#define MOVE_LEFT "cm43"
#define MOVE_RIGHT "cm44"

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN,OUTPUT);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(12);
  vw_setup(4000);// скорость передачи данных в Kbps
}

void loop() {
  int message = Serial.read();
  //send_message(MOVE_FORWARD);
  //delay(1000);
  //send_message(MOVE_BACK);
  //delay(1000);
  digitalWrite(LED_PIN, LOW);
  if (Serial.available() > 0) {
    if (message == 41) {
      send_message(MOVE_FORWARD);
    } else if (message == 42) {
      send_message(MOVE_BACK);
    } else if (message == 43) {
      send_message(MOVE_LEFT);
    } else if (message == 44) {
      send_message(MOVE_RIGHT);
    }
  }
}

void send_message(char *command) {
  char *message = command;
  vw_send((uint8_t *)message, strlen(message));
  vw_wait_tx();
  Serial.println(command);
  digitalWrite(LED_PIN, HIGH);
}

