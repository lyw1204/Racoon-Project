//I2C communications
class Buffer { //implements cyclic buffer for tx and rx
  private:
    int _size;
    byte * _stack;
    byte _top = 0;
    byte _bottom = 0;

  public:
    Buffer(int SIZE) {
      _size = SIZE;
      _stack = new byte [_size];
    }

    void push(byte val) {
      _stack [_top] = val;
      _top ++;
      _top = _top % _size;
    }

    byte pop() {
      byte retVal = _stack[_bottom];
      _bottom++;
      _bottom = _bottom % _size;
      return retVal;
    }

    void bufferFlush() {
      _top = 0;
      _bottom = 0;
    }

    bool isEmpty() {
      return (_bottom == _top);
    }
};

Buffer txBuffer(BUFFER_SIZE);
Buffer rxBuffer(BUFFER_SIZE);
byte slaveState = 0b00000000;
byte cmd = 0b00000000; //Incoming commands from master
byte selfTest = 0b00000000;
bool latestResult = true;
byte unusedDeter = 0;


void slaveBegin() {
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void requestEvent() {
  //write onto I2C
  Wire.write(slaveState);
}

void receiveEvent() {
  cmd = Wire.read();
}


void slaveExecute() {
  Serial.println("cmd");
  Serial.println(cmd);
  Serial.println("slavestate");
  Serial.println(slaveState);

  cmdT = cmd >> 4;//First four bits of cmd
  switch (cmdT) { //Interprets different commands and do different things
    case 0b0000:// Designated standby, will accept command.
      //delay(50);
      break;

    case 0b0001: //self, test
      Serial.println("SELF-TEST DUMMY PROGRAM");
      slaveState = 0b00100000;
      selfTest = 0b00000000; //Fake test for now
      slaveState = 0b00000000;
      cmd = 0;
      break;

    case 0b0010: //scan baseline
      slaveState = 0b01000000;//Scanning baseline
      myScanner.scanFaster(depthBaseline);
      slaveState = 0b00000000;
      cmd = 0;
      break;

    case 0b0011: //scanNow
      slaveState = 0b01100000;
      myScanner.scanFaster(depthNow);
      diffMatrix(depthBaseline, depthNow);//Subtract baseline from depthNow
      medianFilter(depthNow);
      //printMatrix(depthNow);

      latestResult = (judgeMatrix(depthNow));//Store result for reading
      slaveState = 0b00000000;
      cmd = 0;
      break;

    case 0b0100: //scanner home
      slaveState = 0b10000000;//State is now homing..
      myScanner.goHome();
      slaveState = 0b00010000;//Standby, homed
      cmd = 0;
      break;

    case 0b0101: //random alarm
      Serial.println("ALARM!");
      slaveState = 0b10100000;
      ta1.deploy();
      slaveState = slaveState && 0b00011111; //Resets to standby state, unaffected
      cmd = 0;
      break;

    case 0b0111: //E-Stop
      Serial.println("E-Stopped!");
      slaveState = 0b11100000;//Error state
      while (true) {
        //Stop execution here.
      }
      break;

    case 0b1000: //Fetch slaveState,  do nothing
      cmd = 0;
      break;

    case 0b1001://Fetch selfTest data
      slaveState = selfTest;
      cmd = 0;
      break;

    case 0b1011://Fetch latestResult data
      slaveState = latestResult;
      cmd = 0;
      break;

    case 0b1101://Fetch unusedDeter data
      slaveState = ta1.getUnusedDeter();
      cmd = 0;
      break;

    case 0b1111://Finish fetching, reset to standby;
      slaveState = slaveState && 0b00011111;
      cmd = 0;
      break;

    default: //Do nothing
      cmd = 0;
      break;
  }
}
