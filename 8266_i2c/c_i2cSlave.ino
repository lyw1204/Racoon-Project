//I2C communication
class Stack{//implements cyclic buffer for tx and rx
  private:
    int _size;
    byte * _stack;
    byte _top = 0;
    byte _bottom = 0;
    
  public:
    Stack(int SIZE){
      _size = SIZE;
      _stack = new byte [_size];
      }

    void push(byte val){
      _stack [_top] = val;
      _top ++;
      _top = _top % _size;     
      }

    byte pop(){
      byte retVal = _stack[_bottom];
      _bottom++;
      _bottom = _bottom % _size;
      return retVal;
      }

    bool isEmpty(){
      return(_bottom == _top); 
      }
  };
Stack execStack(64);




bool pirFlag = false;

class Slave {
  private:
    uint8_t _address;
    uint8_t _rxBuffer;
    uint8_t _txBuffer;
    byte _healthState;
    bool transmit(byte command) {
      Wire.beginTransmission(_address);
      Wire.write(command);
      Wire.endTransmission();
      _txBuffer = command;
    }

    byte requestSlave() {
      Wire.requestFrom(_address, sizeof(byte));
      byte rx;
      while (Wire.available()) {
        rx = Wire.read();
      }
      return rx;
    }
    
  public:
    Slave(unsigned short address) {
      _address = address;
    }

    void getBaseline() {
      Serial.println("getting baseline now");
      homeScanner();
      transmit(1);
      delay(WAIT_SCAN);//Wait for scan to complete, should change to nonblocking in future
      requestSlave();
      Serial.println("Baseline complete");
      homeScanner();
    }
    bool getDepthNow() {
      Serial.println("Scanning now");
      transmit(2); //Tell arduino to scanNow over I2C
      delay(WAIT_SCAN);//Wait for scan to complete, should change to nonblocking in future
      byte result = requestSlave();
      if (result) {//is human
        return true;
      }
      else { //is racoon
        return false;
      }
    }

    void alarm() {
      transmit(3);
      Serial.println("Alarm Triggered! You are not a human. ");
      delay(WAIT_DETER);
      requestSlave();

    }

    void selfTest() {
      transmit(4);
      delay(500);
      _healthState = requestSlave();
      //_healthState = 0b00001111;
      Serial.print("Health Test:");
      Serial.println(_healthState);

      if (_healthState != 0b00000000) {
        while (true) {
          errorTones();
        }

      }
    }
    void errorTones() {
      byte tempHealth = _healthState;
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

  void bufferFlush(){
    transmit(5);
    Serial.println("Slave buffer flushed");
    //requestSlave();
    }

  void homeScanner(){
    transmit(6);
    Serial.println("Homing scanner");
    delay(WAIT_HOME);    
    
    }

    
};


Slave scanner (SLAVE_ADDR);
