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
      //Return scanner back to its resting position facing back
      goToPos(-15, 0);
      while (digitalRead(this->limSwPin) == HIGH) {
        stepperX->step(1);//Step motor until limit switch is clicked
        delay(15);
      }

      xPos = 0;
      yPos = 0;
      scanner_homed = true;
      delay(300);
    }

    void scanFaster (short matrix [Y_RES][X_RES]) {
      powerOn();
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
      //Stop scanner as soon as scan is over
      scanner_homed = false;
      powerOff();  
      //medianFilter(matrix);//Make it look nice and pretty
    }
};

//Initialize new scanner
Scanner myScanner(STEPPER_P1, STEPPER_P2, STEPPER_P3, STEPPER_P4, STEPS_PER_ROT, STEPPER_SPD, SERVO_P1, LIM_SW1, SONIC_TRIG, SONIC_ECHO, POWER_PIN);
