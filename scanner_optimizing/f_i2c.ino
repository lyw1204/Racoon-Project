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



Buffer rxBuffer(BUFFER_SIZE);

void slaveExecute(){
  byte cmd = rxBuffer.pop();//pop a commannd from rxBuffer
  
  switch (cmd){//Interprets different commands and do different things
    case 0: //scanner home
      myScanner.goHome();
      break;

    case 1: //scan baseline, return 0
      myScanner.scanFaster(depthBaseline);
      break;

    case 2: //scan now, return judgement 1 = human, 0 = raccoon
      myScanner.scanFaster(depthNow);
      diffMatrix(depthBaseline, depthNow);//Subtract baseline from depthNow
      medianFilter(depthNow);
      printMatrix(depthNow);
      break;

    case 3: //random alarm, return 0
      Serial.println("ALARM!");
      ta1.deploy();
      break;

    case 4: //self, test, return 
      Serial.println("SELF-TEST DUMMY PROGRAM");      
      break;

    case 5: //Buffer Flush
      break;

    case 6: //reserved
      break;

    default://Do nothing
      break;

    }
  }
