
#include <Stepper.h>
#include <Servo.h>

//Pin Mapping
#define STEPPER_P1 4
#define STEPPER_P2 6
#define STEPPER_P3 5
#define STEPPER_P4 7

#define SERVO_P1 3


#define LIM_SW1 2

#define SONIC_TRIG 8
#define SONIC_ECHO 9

//Operation Constants
#define STEPS_PER_ROT 2048
#define STEPPER_SPD 7
#define X_RES 27 // size of depth map
#define Y_RES 6

#define X_RANGE 270
#define Y_RANGE 90


//Global variable for depth map accesses
int depthBaseline [Y_RES][X_RES];
int depthNow [Y_RES][X_RES];



//Outputs 2D matrices for nice printing
void printMatrix(int y_size, int x_size, int matrix[Y_RES][X_RES]){
  for(int i = 0; i<y_size; i++){
    for(int j = 0; j<x_size; j++){
      Serial.print(matrix[i][j]);
      Serial.print(' ');
      }
      Serial.print('\n');
    
    }
  
  
  
  }
class Scanner {
  private:
    int xPos = 0; 
    int yPos = 0; 
    bool scanner_en = false;
    bool scanner_homed = false;

    byte stepPin1, stepPin2, stepPin3, stepPin4, stepperSpeed, servoPin, limSwPin, sonicTrigPin, sonicEchoPin;
    int stepsPerRotation;


    Servo servoY;
    Stepper stepperX = Stepper(4096, 4, 5, 6, 7);; //Fake init to appease the library gods



    int getSensor(){
      digitalWrite(sonicTrigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(sonicTrigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(sonicTrigPin, LOW);
      int duration = pulseIn(sonicEchoPin, HIGH);
     
      // Convert the time into a distance
      int cm = (duration/2) / 29.1;     // Divide by 29.1 
      return cm;  
      }

      
  public:
    Scanner(int step1, int step2, int step3, int step4, int stepRot, byte stepSpd, byte serv, byte limSw, byte sonicTrig, byte sonicEcho) {
      this->stepPin1 = step1;
      this->stepPin2 = step2;
      this->stepPin3 = step3;
      this->stepPin4 = step4;
      this->stepsPerRotation = stepRot;
      this->stepperSpeed = stepSpd;
      this->servoPin = serv;
      this-> limSwPin = limSw;
      this->sonicTrigPin = sonicTrig;
      this->sonicEchoPin = sonicEcho; 


      
      this->scanner_en = false;
      this->scanner_homed = false;


      this->stepperX = Stepper(stepRot, step1, step2, step3, step4);
      this->stepperX.setSpeed(STEPPER_SPD);

      //Configure pins
      pinMode(stepPin1, OUTPUT);
      pinMode(stepPin2, OUTPUT);
      pinMode(stepPin3, OUTPUT);
      pinMode(stepPin4, OUTPUT);

      pinMode(servoPin, OUTPUT);

      pinMode(limSwPin, INPUT_PULLUP);

      pinMode(sonicTrigPin, OUTPUT);
      pinMode(sonicEchoPin, INPUT);

    }

    void powerOn() {
      //Connect power to stepper and servos
      this->scanner_en = true;
      servoY.attach(this->servoPin);
    }

    void powerOff() {
      //Disconnect powerto stepper and servos
      this->scanner_en = false;
      servoY.detach();
    }

    void homingInterrupt(){
      
      
      }
    void goHome() {
      this->powerOn();

      this->scanner_homed = false;
      while (digitalRead(this->limSwPin) == HIGH) {
        
        this->stepperX.step(1);//Step motor until limit switch is clicked
        delay(15);
      }

      this->servoY.write(0);

      this->xPos = 0;
      this->yPos = 0;
      this->scanner_homed = true;
    }

    void goToPos(int x, int y) {
      //Connect power to servo and stepper
      this->scanner_en = true;

      //soft guard for invalid positions
      x = x % 360;

 
      if (!this->scanner_homed) {
        //this->goHome();
      }
      this->stepperX.step((x - this->xPos) / (360.0 / this->stepsPerRotation));
      this->servoY.write(y);

      this->xPos = x;
      this->yPos = y;
      this->scanner_homed = false;

    }
    void scan_alt(bool baseline) {//Back and forth scanning through area


      for (int i = 0; i < Y_RES; i++) { //Y axis
        for (int j = 0; j < X_RES; j++) { //X axis
          if (i % 2 == 0) {
            this->goToPos(j * 10, 120-i * 10);
            if(baseline)
              depthBaseline[i][j]=this->getSensor();
            else
              depthNow[i][j]=this->getSensor()-depthBaseline[i][j];

          }
          else {
            this->goToPos((X_RES-1 - j) * 10, 120-i * 10);
            if(baseline)
              depthBaseline[i][X_RES-1-j]=this->getSensor();
            else
              depthNow[i][X_RES-1-j]=this->getSensor()-depthBaseline[i][X_RES-1-j];
          }
          //delay(30);
        }

      }
      //Serial.println("SCAN COMPLETE");
      //Return scanner back to its resting position facing back
      this->goToPos(-45,0);

    }

};


//Initialize new scanner
Scanner myScanner(STEPPER_P1, STEPPER_P2, STEPPER_P3, STEPPER_P4, STEPS_PER_ROT, 5, SERVO_P1, LIM_SW1, SONIC_TRIG, SONIC_ECHO);

//Image processing to find differences and tell whether a target is present
bool targetDetect(){
  
  
  
  
  }

void setup() {
  myScanner.goHome();
  Serial.begin(9600);
  myScanner.scan_alt(true);//Populates a baseline image
  delay(10000);//delay for placement of target
  myScanner.goHome();

  myScanner.scan_alt(false);
  printMatrix(Y_RES,X_RES,depthNow);
  //printMatrix(Y_RES,X_RES,depthBaseline);
  //printMatrix(Y_RES,X_RES,depthNow);


  //Serial<<depthBaseline<<"\n";
  //Subtract(depthNow, depthBaseline,depthDifference);
  //Serial<<depthDifference<<"\n";

}

void loop() {  

}
