class Beacon
{
  private:
    uint8_t _ledPin;
    long _turnOffTime;
    int numReadings = 5;
    int readings[numReadings];      // the readings from the analog input
    int readIndex;              // the index of the current reading
    int total;                  // the running total
    int average;                // the average
    int count;                    //discard first n readings


  public:
    Beacon(uint8_t id);
    //running avrage
    void avg(int t_input);
    //first view of beacon
    void firstView();

    // Turn the led on for a given amount of time (relies on a call to check() in the main loop())
    void setOnForTime(int millis);
    void check();
};

Beacon::Beacon(uint8_t id){
  
}
Beacon::avg(int t_input) {
  total = total - readings[readIndex];
  readings[readIndex] = input;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings)
    readIndex = 0;
  average = total / numReadings;
  return average;
}

