
void tempAlarm(){
        for(int j = 0; j<5; j++){//Sound a squealing alarm, this is NOT 
        for(int i = 1000; i<2000; i++){

          tone(SPEAKER,i);
          delay(1);
          }
        noTone(SPEAKER);
      }
  
  
  
  }

void setup() {
  delay(15000);
  Wire.begin(MASTER_ADDR); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output  
  wifiConnect();
  scanner.bufferFlush();
  scanner.selfTest();
  scanner.getBaseline();
}

void loop() {
  bool isHuman = true; 
  
  if(pirFlag){
    isHuman = scanner.getDepthNow();
    if(!isHuman){
      scanner.alarm();
      Serial.println("ANIMAL");
      tempAlarm();
    }


    else{
      Serial.println("HUMAN");
      }
    firebaseEventPush(isHuman);
    scanner.homeScanner();
    pirFlag = false; 
    }
  /*
  if(!execStack.isEmpty()){
    //Critical sections begin here, not taking any more interrupts. 
    //pir2.disable();
    switch(execStack.pop()){
      case 0:
        //go home:
        break;
      case 1:
        scanner.getBaseline();
        break;
      case 2:
        isRaccoon = scanner.getDepthNow();
        if(isRaccoon){
          execStack.push(3); //If Raccoon, schedule alarm upon detection and pushing
          }
        firebaseEventPush(isRaccoon);
        break;
      case 3:
        scanner.alarm();
        break;
      case 4:
        scanner.selfTest();
        break;
      case 5:
        //reserved
        break;
      case 6:
        //reserved
        break;
      default:
        Serial.println("Invalid value on Stack");
        break;
      }
    }

    */

  

}
