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
#define SONIC_TRIG 8
#define SONIC_ECHO 9

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
#define SAMPLES 5
#define MAXDIST 200


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
  /*
    for(int i = 0; i<Y_RES; i++){//Zero fill the matrix
    for(int j = 0; j<X_RES; j++){
      matrix[i][j] = 0;
      }
    }
  */
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

   if(abs(botRow-topRow) > 3){
    return true;
    }
   else{
    return false; 
    }
  }

//I2C communications handling code





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

    void powerOn() {
      //Connect power to stepper and servos
      scanner_en = true;
      servoY.attach(servoPin);
      digitalWrite(powerPin, HIGH);
    }

    void powerOff() {
      //Disconnect powerto stepper and servos
      this->scanner_en = false;
      servoY.detach();
      digitalWrite(powerPin,LOW);
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

    void goToPos(int x, int y) {
      //Connect power to servo and stepper
      this->scanner_en = true;

      //soft guard for invalid positions
      x = x % 360;


      if (!this->scanner_homed) {
        //this->goHome();
      }
      stepperX->step((x - xPos) / (360.0 / stepsPerRotation));
      servoY.write(y);

      xPos = x;
      yPos = y;
    }


    void scanFaster (short matrix [Y_RES][X_RES]) {
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

    }

};


//Initialize new scanner
Scanner myScanner(STEPPER_P1, STEPPER_P2, STEPPER_P3, STEPPER_P4, STEPS_PER_ROT, STEPPER_SPD, SERVO_P1, LIM_SW1, SONIC_TRIG, SONIC_ECHO, POWER_PIN);

void setup() {
  myScanner.goHome();
  Serial.begin(9600);
  myScanner.scanFaster(depthBaseline);//Populates a baseline image
  delay(10000);
  myScanner.scanFaster(depthNow);

  diffMatrix(depthBaseline, depthNow);//Subtract baseline from depthNow
  medianFilter(depthNow);//Make it look nice and pretty
  printMatrix(depthNow);//printout
  Serial.print(judgeMatrix(depthNow));
  digitalWrite(POWER_PIN, LOW);

}

void loop() {

}
