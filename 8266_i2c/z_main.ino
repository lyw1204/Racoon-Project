
void setup() {
  scanner.errorTones(0b10101010);
  Wire.begin(MASTER_ADDR); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output  
  wifiConnect();
  scanner.selfTest();
  delay(2000);
  scanner.getBaseline();
}

void loop() {
  bool isHuman = true; 
  
  if(pirFlag){
    isHuman = scanner.getDepthNow();
    if(!isHuman){
      scanner.alarm();
      Serial.println("ANIMAL");
      //tempAlarm();
    }
    else{
      Serial.println("HUMAN");
      }
    scanner.homeScanner();
    firebaseEventPush(isHuman);
    pirFlag = false; 
    }
  

}
