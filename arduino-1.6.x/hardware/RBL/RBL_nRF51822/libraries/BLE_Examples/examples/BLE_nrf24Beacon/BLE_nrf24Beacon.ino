//http://doc.lijun.li/misc-nrf24-ble.html
// Inspired by http://dmitry.gr/index.php?r=05.Projects&proj=11.%20Bluetooth%20LE%20fakery

#define DEBUG 1

//this is the ID the beacon will transmit, keep it a number
const static int my_num = 1 ;

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

#define LEDPIN 6


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
int song; //is song playing?

//SMoothing
//int sum = 0 ;
const int numReadings = 5;

int readings2[numReadings];      // the readings from the analog input
int readIndex2 = 0;              // the index of the current reading
int total2 = 0;                  // the running total
int average2 = 0;                // the average
int count02 = 0;                    //discard first n readings
long unsigned lastSeen;
bool firstSeen;

#include "beaconFunctions.h" ///all NRF functions 

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
    Serial.print("restart, my id >> "); Serial.println(my_num);
  }

  mySerial.begin(9600);
  inputString.reserve(50);

  //send comm to MP3 module
  sendCommand(CMD_SEL_DEV, DEV_TF);//select the TF card
  delay(200);//wait for 200ms

  //LED setup
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  //Smoothing
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings2[thisReading] = 0;
  }

  //init beacon
  initNRF();
}

void loop() {
  unsigned long currentMillis = millis();

  if (millis() - previousMillis >= interval) { // Broadcasting interval
    previousMillis = currentMillis;
    channel_hop();

    if ((millis() - lastSeen > 1000) && firstSeen) {
      Serial.println("dead!");
      firstSeen = false;

      song = 0;
      //do soemthing here when beacon goes offline
    }
  }

  serialEventListener(); //get events from serial
  processScan(); //process complete messages from serial
}


void processScan() {
  if (gotBeacon) {
    int rssi = getValue(inputString, '|', 0).toInt();
    int id = getValue(inputString, '|', 1).toInt();

    switch (id) {
      case 2:
        rssi = avrg02(rssi);

        //discard first 10 readings and dont process my own signal!
        count02++ ;
        if (count02 > 10 && id != my_num) {
          if (DEBUG) {
            Serial.print("["); Serial.print(id); Serial.print("]:"); Serial.print(rssi);
            Serial.print(":S"); Serial.println(song);
          }
          player(rssi);
          lastSeen = millis();
          firstSeen = true;
        }
        break;

      default:
        break;
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


void player(int t_rssi) {
  if (t_rssi > 44 && t_rssi <= 50) {
    if (song != 1) {
      //Serial.println("==first song");
      sendCommand(CMD_PLAY_W_VOL, 0x1E01);//play the "1" song with volume 30 class
      digitalWrite(LEDPIN, HIGH);
      song = 1;
    }
  } else if (t_rssi > 50 && t_rssi <= 60) {
    if (song != 2) {
      // Serial.println("==second song");
      sendCommand(CMD_PLAY_W_VOL, 0x1E02);//play the "2" song with volume 30 class
      digitalWrite(LEDPIN, LOW);
      song = 2;
    }
  } else if (t_rssi > 60 && t_rssi <= 75) {
    if (song != 3) {
      // Serial.println("==third song");
      sendCommand(CMD_PLAY_W_VOL, 0x1E03);//play the "3" song with volume 30 class
      digitalWrite(LEDPIN, LOW);
      song = 3;
    }
  } else if (t_rssi <= 44 || t_rssi > 75) {
    song = 0;
    digitalWrite(LEDPIN, LOW);
    //Serial.println("no songs");
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


int avrg02(int input) {
  total2 = total2 - readings2[readIndex2];
  readings2[readIndex2] = input;
  total2 = total2 + readings2[readIndex2];
  readIndex2 = readIndex2 + 1;
  if (readIndex2 >= numReadings)
    readIndex2 = 0;
  average2 = total2 / numReadings;
  return average2;
}
