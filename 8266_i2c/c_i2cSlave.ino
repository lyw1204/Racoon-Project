class Slave {
  private:
    uint8_t _address;
    uint8_t _latestState;
    bool _homed;
    uint8_t _healthState;

    void transmit(byte command) {
      Wire.beginTransmission(_address);
      Wire.write(command);
      Wire.endTransmission();
    }

    byte requestSlave() {
      Wire.requestFrom(_address, sizeof(byte));
      byte rx;
      while (Wire.available()) {
        rx = Wire.read();
      }
      return rx;
    }

    void updateState() { //Updates internal state variables
      byte result = requestSlave();
      Serial.printf("State updated: %d \n", result);
      _latestState = result >> 5;
      _homed = ((result >> 4) && 0b00000001);
    }

    bool waitComm() { //Blocks execution until slave is in standby, or times out after 100s
      delay(500);
      for (int i = 0; i < 100; i++)
      {
        if (requestSlave()<<5 == 0b00000000) {
          Serial.println("scanner is free");
          return true;
        }
        Serial.println("Scanner busy, execution blocked by 1s.");
        delay(1000);
      }
      //Slave timed out, comms has failed
      Serial.print("TIMED OUT, EXECUTION HALTED");
      while(true){
        
        }
      return false;
    }

  public:
    Slave(unsigned short address) {
      _address = address;
      _latestState = 0b11111111; //Uninitialized
      _homed = false;
      _healthState = 0b00000000;//Default no fault
    }

    void selfTest() {
      transmit(0b11110000);
      transmit(0b00010000);//Do selftest
      waitComm();
      transmit(0b10010000);//Fetch test result
      updateState();
      _healthState = _latestState;
      if (_healthState != 0b00000000) {
        while (true) {
          errorTones(_healthState);
        }
      }
      transmit(0b11110000);//Reset to standby
    }

    void errorTones(byte tempHealth) {
      byte mask = 0b00000001;
      for (int i = 0; i < 8; i++) {
        if (tempHealth & mask) { //rightmost bit HIGH
          tone(SPEAKER, 2500);
        }
        else { //rightmost bit low
          tone(SPEAKER, 700);
        }
        delay(500);
        noTone(SPEAKER);
        delay(500);
        tempHealth >>= 1; //shift health bit left
      }
      delay(2000);
    }
    void homeScanner() {
      waitComm();//Wait until scanner is free, grab latest state
      updateState();
      if (_homed) {
        return;
      }
      else {
        Serial.println("Homing scanner");
        transmit(0b01000000);//transmit code to do home
        waitComm();//Wait until scanner is free again
        Serial.println("homing complete");
      }
    }

    void getBaseline() {//scans baseline, and homes scanner when complete
      Serial.println("Baseline scan program begins:");
      waitComm();//Waiting until slave is in standby

      if (!_homed) { //Scanner is NOT homed at beginning
        homeScanner();
      }
      Serial.println("Scanning baseline");
      waitComm();
      transmit(0b00100000);//transmit code to do baseline
      waitComm();
      homeScanner();
    }

    bool getDepthNow() {//Scans now, does NOT home when complete
      Serial.println("ScanNow program begins:");
      waitComm();//wait until scanner in standby state
      updateState();
      if (!_homed) { //Scanner is NOT homed at beginning
        homeScanner();
      }
      Serial.println("Scanning now");
      transmit(0b00110000);//do scanNow
      waitComm();
      transmit(0b10110000);//fetch scanNow result
      byte result = requestSlave();
      transmit(0b11110000);//reset slaveState to standby
      waitComm();
      Serial.println("ScanNow completed");
      if (result) {//is human
        return true;
      }
      else { //is racoon
        return false;
      }
    }

    void alarm() {
      waitComm();
      Serial.println("Alarm Triggered! You are not a human. ");
      transmit(0b01010000);//Do deter
      waitComm();
      transmit(0b11010000);//Load unused deter data
      byte _unusedDeter = requestSlave();//Tells use which one of 3 deterrence is not used
      transmit(0b11110000);//Rest slaveState to standby
      waitComm();
      //Upload alarm record here
    }


    void eStop() { //Emergency stop for scanner
      //Do transmit immediately
      transmit(0b01110000);
      delay(1000);
      updateState();
      if (_latestState == 0b11100000) {
        Serial.println("Emergency stop executed");
      }
      else {
        Serial.println("Emergency stop did not respond");
      }
    }
};


Slave scanner (SLAVE_ADDR);
