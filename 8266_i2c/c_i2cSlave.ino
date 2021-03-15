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
    uint8_t _latestState;
    bool _homed; 
    uint8_t _healthState; 
    
    bool transmit(byte command) {
      Wire.beginTransmission(_address);
      Wire.write(command);
      Wire.endTransmission();
      _txBuffer = command;
    }

    boolean requestSlave() {
      delay(50);//Overhead time for arduino to fetch data into buffer
      Wire.requestFrom(_address, sizeof(byte));
      byte rx;
      while (Wire.available()) {
        rx = Wire.read();
      }
      return rx;
    }

    void updateState(){
      byte result = requestSlave();
      _latestState = result>>5;
      _homed = ((result>>4)&&0b00000001);
      }
    
    void waitComm(){//Blocks execution until slave is in standby, or times out after 100s
      for(int i = 0; i<100; i++)
      {
        transmit(0b10000000);//Fetch state command
        _latestState = reuqestSlave();
        
        if(_latestState  == 0b00000000){
          return true;
          }
        delay(1000);
      }
      //Slave timed out, comms has failed
    return false; 
    }
    
  public:
    Slave(unsigned short address) {
      _address = address;
    }

    void homeScanner() {
      updateState();
      if(_homed){
        return;
        }
      else:{
       waitComm();//Waiting until slave is in standby
        Serial.println("Homing scanner");
       transmit(0b01000000);//transmit code to do home       
        }
      }
    
    void getBaseline() {//scans baseline, and homes scanner when complete
      Serial.println("Waiting for slave");
      waitComm();//Waiting until slave is in standby
      
      if(!_homed){//Scanner is NOT homed at beginning
        homeScanner();
        }
        
      serial.println("Scanning baseline");
      transmit(0b00100000);//transmit code to do baseline

      waitComm();
      homeScanner();
    }
    
    bool getDepthNow() {//Scans now, does NOT home when complete
      waitComm();//wait until scanner in standby state
      if(!_homed){//Scanner is NOT homed at beginning
        homeScanner();
        }
      Serial.println("Scanning now");
      transmit(0b00110000);//do scanNow
      waitComm();
      transmit(0b10110000);//fetch scanNow result
      byte result = requestSlave();
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

      //Upload alarm record here
    }

    void selfTest() {
      waitComm();
      transmit(0b00010000);//Do selftest
      waitComm();
      _healthState = requestSlave();
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

  void bufferFlush(){//Emergency stop for scanner
    //Do transmit immediately
    transmit(0b01110000);
    delay(1000);
    updateState();
    Serial.println("Emergency stop executed");
    }
};


Slave scanner (SLAVE_ADDR);
