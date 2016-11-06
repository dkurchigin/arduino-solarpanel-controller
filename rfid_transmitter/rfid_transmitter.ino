#include <SPI.h>
#include <Ethernet2.h>

#define BUTTON 9

boolean buttonWasUp = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 8, 2); 
IPAddress ip(192, 168, 8, 8);

EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("connecting...");
}

void loop()
{
  boolean buttonUp = digitalRead(BUTTON);
  if (!buttonUp && buttonWasUp) {
    delay(10);
    buttonUp = digitalRead(BUTTON);
    if (!buttonUp) {
      sendRequest();
      delay(100);
      sendPost();
    }
  }
  buttonWasUp = buttonUp;
}

void sendRequest() {
  client.stop();
  if(client.connect(server, 80)) {
    Serial.println("connected");
    client.println("GET /test_params.pl?name=ardu88&color=nicegreen HTTP/1.1");
    client.println("Host: 192.168.8.2");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
}


void sendPost() {
  client.stop();
  char str[] = "name=post88";
  char str2[] = "color=yellow";
  if(client.connect(server, 80)) {
    Serial.println("connected");
    client.println("POST /test_params.pl HTTP/1.1");
    client.println("Host: 192.168.8.2");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println("22");
    client.println();
    client.print("name=post88&color=pink");
    client.println();
  } else {
    Serial.println("connection failed");
  }  
}
