// https://github.com/shenkarSElab/nRF51822-Arduino/tree/S130/arduino-1.6.x/hardware/RBL/RBL_nRF51822/libraries/BLE_Examples/examples/BLE_ScanReport


#include <BLE_API.h>
BLE           ble;


const int numReadings = 5;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void scanCallBack(const Gap::AdvertisementCallbackParams_t *params)
{
  int rawRssi = params->rssi;
  rawRssi = abs(rawRssi);

  if (rawRssi < 90) { //remove spikes
    total = total - readings[readIndex];
    readings[readIndex] = rawRssi;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;

    if (readIndex >= numReadings)
      readIndex = 0;

    average = total / numReadings;
    //Serial.print(rawRssi, DEC); Serial.print(" | ");
    Serial.println(average, DEC);
  }
  /*
    Serial.print(millis());
    Serial.print("|");
    int rawRssi = params->rssi;
    Serial.print(abs(rawRssi), DEC);
    Serial.print("|");

    uint8_t len;
    uint8_t adv_name[31];
    if ( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    Serial.println((const char *)adv_name);
    else if ( NRF_SUCCESS == ble_advdata_decode(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, params->advertisingDataLen, (uint8_t *)params->advertisingData, &len, adv_name) )
    Serial.println((const char *)adv_name);
    else
    Serial.println("no valid name");
  */
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  Serial.println("Start...");
  Serial.println("millis() | RSSI | name");
  ble.init();
  //Note : take care of scheduler, prevent ram leak.See projectconfig.h
  ble.startScan(scanCallBack);
}

void loop()
{
  ble.waitForEvent();
}



uint32_t ble_advdata_decode(uint8_t type, uint8_t advdata_len, uint8_t *p_advdata, uint8_t *len, uint8_t *p_field_data)
{
  uint8_t index = 0;
  uint8_t field_length, field_type;

  while (index < advdata_len)
  {
    field_length = p_advdata[index];
    field_type   = p_advdata[index + 1];
    if (field_type == type)
    {
      memcpy(p_field_data, &p_advdata[index + 2], (field_length - 1));
      *len = field_length - 1;
      return NRF_SUCCESS;
    }
    index += field_length + 1;
  }
  return NRF_ERROR_NOT_FOUND;
}

/*
   //http://forum.arduino.cc/index.php?topic=60560.0
  void modeStat(int num) {
  counts[abs(num)]++;
  // find mode
  uint8_t mode = 0;
  for (uint8_t i = 0; i < 256; i++) {
    mode = (counts[i] > counts[mode]) ? i : mode;
  }
  }
*/
