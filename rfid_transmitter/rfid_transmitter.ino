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

boolean doorIsOpen = false;

boolean buttonWasUp = true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 8, 2); 
IPAddress ip(192, 168, 8, 8);
EthernetClient client;

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
  boolean buttonUp = digitalRead(BUTTON);
  statusPresentation(0);
  if (doorIsOpen) {
    statusPresentation(3);
  }
  
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
    changeSolenoidState(0);
  } else {
    Serial.println("connection failed");
    changeSolenoidState(0);
    //changeSolenoidState(2);
  }  
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}



int sendReedState() {
  int state = digitalRead(REED_SWITCH);
  if (state == 0) {
    doorIsOpen = true;
  } else {
    doorIsOpen = false;
  }
  Serial.println(state);
  return state;
}

void changeSolenoidState(int command) {
  if (command == 0) {
    statusPresentation(1);
    changeSolenoidState(1);
    delay(5000);
  } else if (command == 1) {
    digitalWrite(SOLENOID, LOW);
  } else if (command == 2) {
    digitalWrite(SOLENOID, HIGH); 
  }
  if (sendReedState() == 1) {
      changeSolenoidState(2);
      statusPresentation(0);  
    } else {
      statusPresentation(3);
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
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    tone(BUZZER, 800, 100);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    tone(BUZZER, 800, 100);
    delay(200);
  } else if (command == 4) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    delay(100);
  }
}

