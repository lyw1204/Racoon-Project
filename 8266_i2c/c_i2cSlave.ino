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

    bool getBaseline() {
      Serial.println("getting baseline now");

      transmit(1);
      delay(50000);//Wait 50s for scan to complete, should change to nonblocking in future
      byte result = requestSlave();
      if (result == 0) {
        return true;
      }
      else {
        return false;
      }
    }
    bool getDepthNow() {
      Serial.println("Scanning now");
      transmit(2); //Tell arduino to scanNow over I2C
      delay(50000);//Wait 50s for scan to complete, should change to nonblocking in future
      byte result = requestSlave();
      if (result == 1) {
        return true;
      }
      else {
        return false;
      }
    }

    void alarm() {
      transmit(3);
      delay(5000);
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
          tone(SPEAKER, 2000);

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

};


Slave scanner (SLAVE_ADDR);
