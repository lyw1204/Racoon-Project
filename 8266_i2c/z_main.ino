
void setup() {
  delay(15000);
  Wire.begin(MASTER_ADDR); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output  
  wifiConnect();
  scanner.bufferFlush();
  scanner.selfTest();
  scanner.goHome();
  scanner.getBaseline();
  scanner.goHome();
}

void loop() {
  bool isHuman = true; 
  
  if(pirFlag){
    isHuman = scanner.getDepthNow();
    if(!isHuman){
      scanner.alarm();
      Serial.println("ANIMAL");
    }

    else{
      Serial.println("HUMAN");
      }
    firebaseEventPush(isHuman);
    pirFlag = false; 
    scanner.goHome();
    }

  

}
