//http://doc.lijun.li/misc-nrf24-ble.html
// Inspired by http://dmitry.gr/index.php?r=05.Projects&proj=11.%20Bluetooth%20LE%20fakery

#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#define PIN_CE  10 // chip enable
#define PIN_CSN 9   // chip select (for SPI)


#define my_num   '2'

// The MAC address of BLE advertizer -- just make one up
#define MY_MAC_0  0xa1
#define MY_MAC_1  0x12
#define MY_MAC_2  0x25
#define MY_MAC_3  0x5d
#define MY_MAC_4  0x55
#define MY_MAC_5  0xf7


#include "funcs.h" ///all NRF functions 

#include "SoftwareSerial.h"
SoftwareSerial mySerial(8, 7); //RX,TX
String inputString = "";         // a string to hold incoming data
int beacon[3] = {0, 0, 0};
bool gotBeacon = false;  // whether the string is complete
bool validId = false;

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  Serial.begin(9600);
  Serial.println("restart serial");


  mySerial.begin(9600); //serial with only RX pin
  inputString.reserve(50);
  mySerial.println("restart");

  pinMode(PIN_CSN, OUTPUT);
  pinMode(PIN_CE, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(PIN_CSN, HIGH);
  digitalWrite(PIN_CE, LOW);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);

  initNRF();



}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Broadcasting interval
    previousMillis = currentMillis;
    channel_hop();
  }
  processScan();
}

void processScan() {
  if (gotBeacon) {
    int rssi = getValue(inputString, '|', 0).toInt();
    int id = getValue(inputString, '|', 1).toInt();

    switch (id) {
      case 1:
        beacon[1] = rssi;
        validId = true;
        break;
      case 2:
        beacon[2] = rssi;
        validId = true;
        break;
      case 3:
        beacon[3] = rssi;
        validId = true;
        break;
      default:
        break;
    }
    if (validId) {
      Serial.print("["); Serial.print(id);
      Serial.print("]:"); Serial.println(rssi);
      validId = false;
    }
    // clear the string:
    inputString = "";
    gotBeacon = false;

  }
  serialEventListener();
}

void serialEventListener() {
  while (mySerial.available()) {
    char inChar = (char)mySerial.read();

    inputString += inChar;
    if (inChar == '#') {
      gotBeacon = true;
    }
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}



