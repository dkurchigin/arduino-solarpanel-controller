#include <SPI.h>
#include <Ethernet2.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)
IPAddress ip(192, 168, 8, 8);
EthernetClient client;

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  //}
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");  
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.connect(server, 80)) {
    sentRequest;
  }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  digitalWrite(13, LOW);
  delay(5000);
  // if the server's disconnected, stop the client:
  /*if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }*/
}

void sentRequest() {
  //Serial.println("connected");
  // Make a HTTP request:
  digitalWrite(13, HIGH);
  client.println("GET /search?q=arduino HTTP/1.1");
  client.println("Host: www.google.com");
  client.println("Connection: close");
  client.println();
}

