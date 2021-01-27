



PIR pir1 = PIR_L;//PIR Initialized
PIR pir2 = PIR_R;

void setup() {
  delay(1500);
  Wire.begin(MASTER_ADDR); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output  
  scanner.selfTest();
  scanner.getBaseline();
  delay(30000);

  //now enable interrupt pir 
  pir1.enable();
  pir2.enable();

  
  wifiConnect();

}

void loop() {

  

}
