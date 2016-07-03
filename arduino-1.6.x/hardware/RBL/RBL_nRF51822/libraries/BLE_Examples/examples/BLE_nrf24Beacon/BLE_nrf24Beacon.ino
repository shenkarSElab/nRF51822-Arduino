//http://doc.lijun.li/misc-nrf24-ble.html
// Inspired by http://dmitry.gr/index.php?r=05.Projects&proj=11.%20Bluetooth%20LE%20fakery

//this is the ID the beacon will transmit, keep it a number
int my_num = 2 ;

// The MAC address of BLE advertizer -- just make one up
#define MY_MAC_0  0xa1
#define MY_MAC_1  0x12
#define MY_MAC_2  0x25
#define MY_MAC_3  0x5d
#define MY_MAC_4  0x55
#define MY_MAC_5  0xf7

#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#define PIN_CE  10 // chip enable
#define PIN_CSN 9   // chip select (for SPI)


#include "SoftwareSerial.h"
SoftwareSerial mySerial(8, 7); //RX,TX
static int8_t Send_buf[8] = {0} ;


int beacon[3] = {0, 0, 0};
bool gotBeacon = false;  // whether the string is complete
bool validId = false;
String inputString = "";         // a string to hold incoming data

/// MP3 commands
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY_W_VOL 0X22
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PREVIOUS 0X02
#define CMD_NEXT 0X01


//SMoothing
//int sum = 0 ;
const int numReadings = 5;
int readings1[numReadings];      // the readings from the analog input
int readIndex1 = 0;              // the index of the current reading
int total1 = 0;                  // the running total
int average1 = 0;                // the average
int count01;                      //discard first n readings

int readings2[numReadings];      // the readings from the analog input
int readIndex2 = 0;              // the index of the current reading
int total2 = 0;                  // the running total
int average2 = 0;                // the average
int count02;                      //discard first n readings

#include "beaconFunctions.h" ///all NRF functions 

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  Serial.begin(9600);
  Serial.println("-restart serial");

  mySerial.begin(9600); //serial with only RX pin
  inputString.reserve(50);
  mySerial.println("restart");

  //send comm to MP3 module
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card
  delay(200);//wait for 200ms

  //LED setup
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  //Smoothing
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings1[thisReading] = 0;
  }
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings2[thisReading] = 0;
  }

  //init beacon
  initNRF();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) { // Broadcasting interval
    previousMillis = currentMillis;
    channel_hop();
  }

  serialEventListener(); //get events from serial
  processScan(); //process complete messages from serial
}


void processScan() {
  if (gotBeacon) {
    int rssi = getValue(inputString, '|', 0).toInt();
    int id = getValue(inputString, '|', 1).toInt();
    int rssiAvg;
    switch (id) {
      case 1:
        beacon[1] = rssi;

        total1 = total1 - readings1[readIndex1];
        readings1[readIndex1] = rssi;
        total1 = total1 + readings1[readIndex1];
        readIndex1 = readIndex1 + 1;
        if (readIndex1 >= numReadings) {
          readIndex1 = 0;
        }
        average1 = total1 / numReadings;
        rssi = average1;

        //discard first 10 readings
        count01++ ;
        if (count01 > 10)
          validId = true;

        break;

      case 2:
        beacon[2] = rssi;

        total2 = total2 - readings2[readIndex2];
        readings2[readIndex2] = rssi;
        total2 = total2 + readings2[readIndex2];
        readIndex2 = readIndex2 + 1;
        if (readIndex2 >= numReadings) {
          readIndex2 = 0;
        }
        average2 = total2 / numReadings;
        rssi = average2;

        //discard first 10 readings and dont process my own signal!
        count02++ ;
        if (count02 > 10 || id != my_num)
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


void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for (uint8_t i = 0; i < 8; i++) //
  {
    mySerial.write(Send_buf[i]) ;
  }
}
