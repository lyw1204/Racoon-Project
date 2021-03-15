
void setup() {
  Serial.begin(9600);
  
  slaveBegin();
}

void loop() {
  while(!rxBuffer.isEmpty()){
    slaveExecute();    
    }
}
