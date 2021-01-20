#include <Wire.h>
#include <Stepper.h>
#include <Servo.h>
#include <NewPing.h>


//Pin Mapping
#define STEPPER_P1 4
#define STEPPER_P2 6
#define STEPPER_P3 5
#define STEPPER_P4 7
#define SERVO_P1 3
#define LIM_SW1 2
#define SONIC_TRIG 9
#define SONIC_ECHO 8

#define POWER_PIN 10 

//Operation Constants
#define STEPS_PER_ROT 2048
#define STEPPER_SPD 7

//UNUSED
#define X_RANGE 270
#define Y_RANGE 90

//Offsets and backlash compensations
#define Y_OFFSET 170
#define BACKLASH 5 //degrees of gear backlash



//Matrix Parameters
#define X_RES 27 // size of depth map is 3888 bits
#define Y_RES 9
#define SAMPLES 3
#define MAXDIST 200

//I2C Configuration
#define SLAVE_ADDR 8
#define BUFFER_SIZE 16



//Global variable for depth map accesses
short depthBaseline [Y_RES][X_RES];
short depthNow [Y_RES][X_RES]; 
short processing [Y_RES - 2][X_RES - 2]; //Temporary matrix



//Outputs 2D matrices for nice printing
void printMatrix(short matrix[Y_RES][X_RES]) {
  for (int i = 0; i < Y_RES; i++) {
    for (int j = 0; j < X_RES; j++) {
      Serial.print(matrix[i][j]);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
}

//Filtering functions

void diffMatrix(short matrix1[Y_RES][X_RES], short matrix2 [Y_RES][X_RES]) { //In place matrix difference done on matrix 2
  for (int i = 0; i < Y_RES; i++) {
    for (int j = 0; j < X_RES; j++) {
      matrix2[i][j] -= matrix1[i][j];
    }
  }
}


short median9 (short arr[9]) { //Bubble sort and return middle
  short temp = 0;
  for (int j = 0; j < 9; j++) {
    for (int i = 0; i < 8; i++) {

      if (arr[i] > arr[i + 1]) {
        temp = arr[i];
        arr[i] = arr[i + 1];
        arr[i + 1] = temp;
      }
    }
  }
  return arr[4];
}
void medianFilter(short matrix[Y_RES][X_RES]) {//Applies median filter on matrix
  short neighbors [9];

  for (int i = 0; i < Y_RES - 2; i++) { //Populate processing matrix using medians of 9 neighbors
    for (int j = 0; j < X_RES - 2; j++) {
      //Populates 9 neighbors
      neighbors[0] = matrix[i][j];
      neighbors[1] = matrix[i + 1][j];
      neighbors[2] = matrix[i + 2][j];
      neighbors[3] = matrix[i][j + 1];
      neighbors[4] = matrix[i + 1][j + 1];
      neighbors[5] = matrix[i + 2][j + 1];
      neighbors[6] = matrix[i][j + 2];
      neighbors[7] = matrix[i + 1][j + 2];
      neighbors[8] = matrix[i + 2][j + 2];

      processing[i][j] = median9(neighbors);

    }
  }
  //Edge pixels will remain unchanged
  for (int i = 1; i < Y_RES - 1; i++) {
    for (int j = 1; j < X_RES - 1; j++) { //Change all pixels but the outer edge
      matrix[i][j] = processing[i - 1][j - 1];


    }

  }
}


bool judgeMatrix(short matrix[Y_RES][X_RES]){//Returns true if human, else false
  short colAccum[Y_RES-2];
  char topRow = 0;
  char botRow = 0;
  
  for(short i = 1; i<Y_RES-1; i++){
    colAccum[i-1] = 0;//Initializes colAccum array elements
    
    for(short j = 1; j<X_RES-1; j++){
      if (matrix[i][j]<0){//Count less-than-zero pixels for every row and store in colAccum
        colAccum[i-1] ++;        
        }
      }
    if(colAccum[i-1]>4){//Write 1 if more than 4 pixels are true for that line, else write 0
      colAccum[i-1] = 1;
      }
    else{
      colAccum[i-1] = 0;
      }
    Serial.println(colAccum[i-1]);
    }

  for(char i = 0; i<Y_RES-2; i++){
    if(colAccum[i]){
      topRow = i;
      break;
      }
    }
   for(char i = Y_RES-2-1; i>=0; i--){

    if(colAccum[i]){
      botRow = i;
      break;
      }
    } 

   if(abs(botRow-topRow) >= 3){
    return true;
    }
   else{
    return false; 
    }
  }



class Scanner {
  private:
    int xPos = 0;
    int yPos = 0;
    bool scanner_en = false;
    bool scanner_homed = false;

    byte stepPin1, stepPin2, stepPin3, stepPin4, stepperSpeed, servoPin, limSwPin, sonicTrigPin, sonicEchoPin, powerPin;
    int stepsPerRotation;


    Servo servoY;
    Stepper *stepperX = 0;
    NewPing *scanner = 0;

    short avgSensor(int runs) {
      delay(50);//Wait out mechanical vibrations
      int result = scanner->convert_cm (scanner->ping_median(runs));

      if (result == 0) {
        return MAXDIST;
      }
      else {
        return result;
      }
    }
    
    
    void goToPos(int x, int y) {

      //soft guard for invalid positions
      x = x % 360;

      stepperX->step((x - xPos) / (360.0 / stepsPerRotation));
      servoY.write(y);

      xPos = x;
      yPos = y;
    }
    void powerOn() {
      //Connect power to stepper and servos
      scanner_en = true;
      servoY.attach(servoPin);
      digitalWrite(powerPin, HIGH);
    }

    void powerOff() {
      //Disconnect powerto stepper and servos
      scanner_en = false;
      servoY.detach();
      digitalWrite(powerPin,LOW);
    }

  public:
    Scanner(int step1, int step2, int step3, int step4, int stepRot, byte stepSpd, byte serv, byte limSw, byte sonicTrig, byte sonicEcho, byte power) {
      stepPin1 = step1;
      stepPin2 = step2;
      stepPin3 = step3;
      stepPin4 = step4;
      stepsPerRotation = stepRot;
      stepperSpeed = stepSpd;
      servoPin = serv;
      limSwPin = limSw;
      sonicTrigPin = sonicTrig;
      sonicEchoPin = sonicEcho;

      powerPin = power;

      scanner_en = false;
      scanner_homed = false;


      stepperX = new Stepper(stepRot, step1, step2, step3, step4);
      stepperX->setSpeed(STEPPER_SPD);

      scanner = new NewPing(sonicTrigPin, sonicEchoPin, MAXDIST);

      //Configure pins
      pinMode(stepPin1, OUTPUT);
      pinMode(stepPin2, OUTPUT);
      pinMode(stepPin3, OUTPUT);
      pinMode(stepPin4, OUTPUT);

      pinMode(servoPin, OUTPUT);

      pinMode(limSwPin, INPUT_PULLUP);

      pinMode(sonicTrigPin, OUTPUT);
      pinMode(sonicEchoPin, INPUT);

      pinMode(POWER_PIN, OUTPUT);

    }

    void goHome() {
      powerOn();
      servoY.write(Y_OFFSET);
      while (digitalRead(this->limSwPin) == HIGH) {
        this->stepperX->step(1);//Step motor until limit switch is clicked
        delay(15);
      }


      xPos = 0;
      yPos = 0;
      scanner_homed = true;
      delay(300);
    }

    void scanFaster (short matrix [Y_RES][X_RES]) {
      powerOn();
      goHome();
      for (int j = 0; j < X_RES; j++) {
        for (int i = 0; i < Y_RES; i++) {
          if (j % 2 == 0) {
            goToPos(j * 10, Y_OFFSET - i * 10);
            matrix[i][j] = avgSensor(SAMPLES);
          }
          else {
            goToPos(j * 10, Y_OFFSET - (Y_RES - 1 - i) * 10);
            matrix[Y_RES - 1 - i][j] = avgSensor(SAMPLES);
          }
        }

      }
      //Return scanner back to its resting position facing back
      goToPos(-45, 0);
      scanner_homed = false;
      powerOff();  
      medianFilter(matrix);//Make it look nice and pretty
    }
};

//Initialize new scanner
Scanner myScanner(STEPPER_P1, STEPPER_P2, STEPPER_P3, STEPPER_P4, STEPS_PER_ROT, STEPPER_SPD, SERVO_P1, LIM_SW1, SONIC_TRIG, SONIC_ECHO, POWER_PIN);


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

    bool isEmpty(){
      return(_bottom == _top); 
      }
  };

void slaveBegin(){
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  }


Buffer txBuffer(BUFFER_SIZE);
Buffer rxBuffer(BUFFER_SIZE);

void requestEvent(){
  //Pop txBuffer and write onto I2C
  Wire.write(txBuffer.pop());
  }

void receiveEvent(){
  rxBuffer.push(Wire.read());
  }

void slaveExecute(){
  byte cmd = rxBuffer.pop();//pop a commannd from rxBuffer
  
  switch (cmd){//Interprets different commands and do different things
    case 0: //scanner home
      myScanner.goHome();
      txBuffer.push(0);
      break;

    case 1: //scan baseline, return 0
      myScanner.scanFaster(depthBaseline);
      txBuffer.push(0);
      break;

    case 2: //scan now, return judgement 1/0
      myScanner.scanFaster(depthNow);
      diffMatrix(depthBaseline, depthNow);//Subtract baseline from depthNow
      txBuffer.push(judgeMatrix(depthNow));
      break;

    case 3: //random alarm, return 0
      Serial.println("ALARM!");
      txBuffer.push(0);
      break;

    case 4: //self, test, return 
      Serial.println("SELF-TEST DUMMY PROGRAM");
      txBuffer.push(0b00000000); //Fake test for now
      
      break;

    case 5: //reserved
      txBuffer.push(0);
      break;

    case 6: //reserved
      txBuffer.push(0);
      break;

    default://Do nothing
      txBuffer.push(0b11111111);//0b11111111 indicates trransmission failure to 8266, host should retransmit
      break;

    }
  }


void setup() {
  Serial.begin(9600);
  slaveBegin();

}

void loop() {
  while(rxBuffer.isEmpty()==0){
    slaveExecute();    
    }
}
