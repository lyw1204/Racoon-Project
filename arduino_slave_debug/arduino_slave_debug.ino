#include <Wire.h>

// Wire Slave Receiver
// by devyte
// based on the example by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// This example code is in the public domain.


#define SDA_PIN 4
#define SCL_PIN 5
#define LED_PIN 9
#define LED_PIN2 12

const int16_t I2C_MASTER = 0x42;
const int16_t I2C_SLAVE = 0x08;
byte rxBuffer = 0;
byte txBuffer = 0;

void faketask() {
  Serial.println("1 start");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  txBuffer = 11;
  Serial.println("1 end");
  rxBuffer = 0;//Transition to standby state
  
}

void faketask2(){
  Serial.println("2 start");

  digitalWrite(LED_PIN2, HIGH);
  delay(1000);
  digitalWrite(LED_PIN2, LOW);
  delay(1000);
  txBuffer = 22; 
  Serial.println("2 end");
  rxBuffer = 0;//Transition to standby state


  }


void setup() {
  Serial.begin(9600);           // start serial for output
  Wire.begin(I2C_SLAVE); // new syntax: join i2c bus (address required for slave)
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
}

void loop() {
  executor(rxBuffer);
  
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent() {
  rxBuffer = Wire.read();
  Serial.println(rxBuffer);
}


//Sends the txBuffer on request interrupt
void requestEvent() {
  Serial.println("tx start");
  Serial.println(txBuffer);
  Wire.write(txBuffer);
  Serial.println("tx end");

}


void executor(byte command){
  switch (command) {
    case 0: //Standby
      break;
    case 1: //FakeTask
      Serial.println("Case 1");
      faketask();
      break;
    case 2:
      Serial.println("Case 2");
      faketask2();
      break;
    default:
      Serial.println("default");
      break;

  } 
  }
