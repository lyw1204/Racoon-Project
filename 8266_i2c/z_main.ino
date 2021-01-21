

Slave scanner (SLAVE_ADDR);
PIR pir1 = (PIR_L);//Left PIR Initialized


void setup() {
  delay(1500);
  Wire.begin(MASTER_ADDR); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  scanner.selfTest();

  scanner.getBaseline();
  pir1.enable();//PIR Interrupt starts working

}

void loop() {
  Serial.println(scanner.getDepthNow());
  delay(10000);

}
