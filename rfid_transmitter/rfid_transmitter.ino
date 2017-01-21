#include <SPI.h>
#include <Ethernet2.h>

#define BUTTON 9
#define BUZZER 3
#define GREEN_LED 5
#define RED_LED 4

boolean buttonWasUp = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 8, 2); 
IPAddress ip(192, 168, 8, 8);

EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);    
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);  

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

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
      digitalWrite(GREEN_LED, HIGH);
      sendRequest();
      delay(100);
      sendPost();
      digitalWrite(GREEN_LED, LOW);
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
    tone(BUZZER, 523, 200);
  } else {
    Serial.println("connection failed");
  }  
}
