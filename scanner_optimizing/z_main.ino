
void setup() {
  Serial.begin(9600);
}

void loop() {
  rxBuffer.push(1);
  while(!rxBuffer.isEmpty()){
    slaveExecute();    
    }
}
