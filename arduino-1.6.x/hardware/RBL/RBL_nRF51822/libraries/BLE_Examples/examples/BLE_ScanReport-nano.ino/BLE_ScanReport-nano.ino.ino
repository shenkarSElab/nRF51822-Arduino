//find me on https://github.com/shenkarSElab/nRF51822-Arduino/tree/S130/arduino-1.6.x/hardware/RBL/RBL_nRF51822/libraries/BLE_Examples/examples


String inputString = "";         // a string to hold incoming data
boolean gotBeacon = false;  // whether the string is complete

//here we store our beacons. index is thier ID
int beacon[] = {0, 0, 0}; //here we store the beacons
boolean beaconAlive[] = {0, 0, 0}; //are we getting this beacon?
int beaconArraySize = 3;

void setup() {
  Serial.begin(9600);
  inputString.reserve(50);
}

void loop() {
  if (gotBeacon) {
    int rssi = getValue(inputString, '|', 0).toInt();
    int id = getValue(inputString, '|', 1).toInt();
    //we store the beacon RSSI in an array
    beacon[id] = rssi;
    //this is live
    beaconAlive[id] = true;

    for (int iii = 1; iii < beaconArraySize; iii++) {
      if (beaconAlive[iii]) {
        Serial.print("beacon["); Serial.print(iii); Serial.print("]:"); Serial.print(beacon[iii]); Serial.println();
        //reset the alive switch
        beaconAlive[iii] = false;
      }

      // clear the string:
      inputString = "";
      gotBeacon = false;
    }
  }
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
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


