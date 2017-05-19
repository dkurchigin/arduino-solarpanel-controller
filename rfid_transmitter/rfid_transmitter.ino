#include <SPI.h>
#include <Ethernet2.h>
#include <MFRC522.h>

#define BUTTON 9
#define BUZZER 3
#define GREEN_LED 5
#define RED_LED 4
#define REED_SWITCH 6
#define SOLENOID A2
#define ANOTHER_GND A3
#define SS_PIN 8
#define RST_PIN 7
#define bufferMax 128
char buffer[bufferMax];

char unique_id[] = "xkkshvq3l9";
boolean doorIsOpen = false;
int state = 0;   /*   0-ready
                      1-success
                      2-failed
                      3-opened
*/

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress pi_server(192, 168, 8, 2); 
IPAddress ip(192, 168, 8, 8);
EthernetClient client;
EthernetServer server(80);

MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;
byte nuidPICC[3];


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT);
  pinMode(BUZZER, OUTPUT);    
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);  
  pinMode(REED_SWITCH, INPUT);
  pinMode(SOLENOID, OUTPUT);
  pinMode(ANOTHER_GND, OUTPUT);
  digitalWrite(SOLENOID, HIGH);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(ANOTHER_GND, LOW);

  Ethernet.begin(mac, ip);
  delay(300);
  server.begin();
  delay(1000);
  Serial.println("connecting...");

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
}


void loop() {
  readReedSwitch();
  client = server.available();
  getPostRequest();

  if (state == 0) {
    statusPresentation(0);
    changeSolenoidState(1);
  } else if (state == 1) {
    statusPresentation(1);
    changeSolenoidState(0);
  } else if (state == 2) {
    statusPresentation(2);
    changeSolenoidState(1);
  } else if (state == 3) {
    statusPresentation(3);
    changeSolenoidState(0);
  }


  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    sendPost();
  }
  else {
    Serial.println(F("Card read previously."));
    sendPost();
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
}

/*void sendRequest() {
  client.stop();
  if(client.connect(pi_server, 80)) {
    Serial.println("connected");
    client.println("GET /test_params.pl?name=ardu88&color=nicegreen HTTP/1.1");
    client.println("Host: 192.168.8.2");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
}*/

void sendPost() {
  client.stop();
  char str[] = "name=post88";
  char str2[] = "color=yellow";
  if(client.connect(pi_server, 80)) {
    Serial.println("connected");
    client.println("POST /test_params.pl HTTP/1.1");
    client.println("Host: 192.168.8.2");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println("34"); //old 22 previos 28
    client.println();
    client.print("reader=");
    client.print(unique_id);
    client.print("&nuid=");
    client.print(rfid.uid.uidByte[0]);
    client.print(rfid.uid.uidByte[1]);
    client.print(rfid.uid.uidByte[2]);
    client.print(rfid.uid.uidByte[3]);
    client.println();
    changeSolenoidState(0);
    statusPresentation(1);
  } else {
    Serial.println("connection failed");
    changeSolenoidState(0);
    statusPresentation(1);
  }  
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void readReedSwitch() {
  int reedState = digitalRead(REED_SWITCH);
  if (reedState == 0) {
    doorIsOpen = true;
  } else {
    doorIsOpen = false;
  }
}

void changeSolenoidState(int command) {
  if (command == 0) {
    digitalWrite(SOLENOID, LOW);
    delay(200);
  } else if (command == 1) {
    digitalWrite(SOLENOID, HIGH); 
  }
}

void statusPresentation(int command) {
  if (command == 0) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);    
  } else if (command == 1) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    tone(BUZZER, 700, 200);
  } else if (command == 2) {
    tone(BUZZER, 500, 200);
  } else if (command == 3) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    tone(BUZZER, 800, 100);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    tone(BUZZER, 800, 100);
    delay(200);
  } 
}

void getPostRequest() {
  if (client) {
    boolean currentLineIsBlank = true;
    int bufferSize = 0;
    while (client.connected()) {
      if(client.available()){
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          while(client.available()) {  
            char post = client.read();   
            if(bufferSize < bufferMax)
              buffer[bufferSize++] = post;  // сохраняем новый символ в буфере и создаем приращение bufferSize 
          }
          Serial.println("Received POST request:");
          // Разбор HTTP POST запроса                  
          //ParseReceivedRequest();
          // Выполнение команд
          //PerformRequestedCommands();
          // Отправка ответа
          //sendResponse();
          //changeSolenoidState(1);
          //statusPresentation(1);
          state = 1;
          sendResponse();
        } 
        else if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
  }
}

void sendResponse() {
  Serial.println("Sending response");
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.stop();
  delay(200);
}
