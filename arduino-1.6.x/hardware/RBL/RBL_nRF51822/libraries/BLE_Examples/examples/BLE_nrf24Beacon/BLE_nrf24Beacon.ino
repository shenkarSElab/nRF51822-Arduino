//http://doc.lijun.li/misc-nrf24-ble.html
// Inspired by http://dmitry.gr/index.php?r=05.Projects&proj=11.%20Bluetooth%20LE%20fakery


//make sure to change the MAC (dont go to the end, change at middle?!)
// and the name


#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#define PIN_CE  10 // chip enable
#define PIN_CSN 9   // chip select (for SPI)

// The MAC address of BLE advertizer -- just make one up
#define MY_MAC_0  0xa1
#define MY_MAC_1  0x12
#define MY_MAC_2  0x25
#define MY_MAC_3  0x5d
#define MY_MAC_4  0x55
#define MY_MAC_5  0xf7

#define my_num   2

#include "funcs.h" ///all NRF functions 

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  pinMode(PIN_CSN, OUTPUT);
  pinMode(PIN_CE, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(PIN_CSN, HIGH);
  digitalWrite(PIN_CE, LOW);

  Serial.begin(9600);
  Serial.println("Start LE advertizing");
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
}

