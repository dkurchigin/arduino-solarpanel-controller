#include <VirtualWire.h>

void setup() {
  Serial.begin(9600);
  vw_set_ptt_inverted(true); // необходимо для модуля DR3100
  vw_set_rx_pin(12);
  vw_setup(4000); // бит в секунду
  pinMode(13, OUTPUT);
  vw_rx_start(); // запуск фазовой автоподстройки частоты (ФАПЧ)
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) {  // Не блокируется
    if (buf[0] == 'c' && buf[1] == 'm') {
      digitalWrite(13,1);
      Serial.println("got command");
    }
      if (buf[0]=='0') {
      digitalWrite(13,0);
      Serial.println("have not command");
    }
  }
}
