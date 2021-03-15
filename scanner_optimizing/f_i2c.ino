//I2C communications
class Buffer{//implements cyclic buffer for tx and rx
  private:
    int _size;
    byte * _stack;
    byte _top = 0;
    byte _bottom = 0;
    
  public:
    Buffer(int SIZE){
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

    void bufferFlush(){
      _top = 0;
      _bottom = 0;      
      }

    bool isEmpty(){
      return(_bottom == _top); 
      }
  };


Buffer txBuffer(BUFFER_SIZE);
Buffer rxBuffer(BUFFER_SIZE);
byte slaveState =0b00000000;
byte cmd = 0b00000000; //Incoming commands from master
byte selfTest = 0b00000000;
bool latestResult = true; 
byte unusedDeter = 0;

void slaveBegin(){
  
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  }

void requestEvent(){
  //write onto I2C
  Wire.write(slaveState);
  }

void receiveEvent(){
  cmd = Wire.read();
  }

void slaveExecute(){
  cmd = cmd >> 4;
  switch (cmd){//Interprets different commands and do different things
    case 0b0000:// RSV No command this cycle
      break;
      
    case 0b0001: //self, test
      Serial.println("SELF-TEST DUMMY PROGRAM");
      slaveState = 00100000;
      selfTest = 0b00000000; //Fake test for now
      slaveState = 0b00000000;
      break;
    case 0b0010: //scan baseline
      slaveState = 0b01000000;//Scanning baseline
      myScanner.scanFaster(depthBaseline);
      slaveState = 0b00010000;
      break;

    case 0b0011: //scanNow
      slaveState = 0b01100000;
      myScanner.scanFaster(depthNow);
      diffMatrix(depthBaseline, depthNow);//Subtract baseline from depthNow
      medianFilter(depthNow);
      //printMatrix(depthNow);
      
      latestResult = (judgeMatrix(depthNow));//Store result for reading
      slaveState = 0b00000000;
      break;
      
    case 0b0100: //scanner home
      slaveState = 0b10000000;//State is now homing..
      myScanner.goHome();
      slaveState = 0b00010000;//Standby, homed
      break;
      
    case 0b0101: //random alarm
      Serial.println("ALARM!");
      slaveState = 0b10100000;
      ta1.deploy();
      slaveState = slaveState || 0b00011111; //Resets to standby state, unaffected
      break;

    case 0b1000: //Fetch slaveState, meaning do nothing
      break;

    case 0b1001://Fetch selfTest data
      slaveState = selfTest;
      break;

    case 0b1011://Fetch latestResult data
      slaveState = latestResult;
      break;

    case 0b1101://Fetch unusedDeter data
      slaveState = ta1.getUnusedDeter();
      break;

    default: //Do nothing
      break;
    }
    cmd = 0;//Clears cmd after executing
  }
