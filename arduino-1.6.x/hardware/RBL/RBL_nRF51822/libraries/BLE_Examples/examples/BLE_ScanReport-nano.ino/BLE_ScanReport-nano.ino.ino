//find me on https://github.com/shenkarSElab/nRF51822-Arduino/tree/S130/arduino-1.6.x/hardware/RBL/RBL_nRF51822/libraries/BLE_Examples/examples


String inputString = "";         // a string to hold incoming data
int beacon[3] = {0, 0, 0};
bool gotBeacon = false;  // whether the string is complete
bool validId = false;

void setup() {
  Serial.begin(9600);
  inputString.reserve(50);
  Serial.println("restart");
}

void loop() {
  if (gotBeacon) {
    //Serial.println(inputString);
    int rssi = getValue(inputString, '|', 0).toInt();
    int id = getValue(inputString, '|', 1).toInt();
    //Serial.print("id:"); Serial.println(id);

    switch (id) {
      case 1:
        beacon[1] = rssi;
        validId = true;
        break;
      case 2:
        beacon[2] = rssi;
        validId = true;
        break;
      default:
        break;
    }
    if (validId) {
      Serial.print("beacon["); Serial.print(id);
      Serial.print("]:"); Serial.println(rssi);
      validId = false;
    }
    // clear the string:
    inputString = "";
    gotBeacon = false;

  }
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
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


