#include <SPI.h>
#include <Ethernet2.h>

int i = 0;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 8, 2);  // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)
IPAddress ip(192, 168, 8, 8);
EthernetClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("connecting...");
}

void loop()
{
  delay(1000);
  sendRequest();
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

