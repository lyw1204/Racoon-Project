#define SCL D1
#define SDA D2
#define IO_LED D7

#define MASTER_ADDR 1
#define SLAVE_ADDR 8

#include <Wire.h>


class Slave {
  private:
    uint8_t _address;
    uint8_t _rxBuffer;
    uint8_t _txBuffer;
    uint8_t _state;






  public:
    Slave(unsigned short address) {
      _address = address;

    }

    bool transmit(byte command) {
      digitalWrite(IO_LED, HIGH);
      Wire.beginTransmission(_address);
      Wire.write(command);
      Wire.endTransmission();
      _txBuffer = command;
      digitalWrite(IO_LED, LOW);
    }

    bool requestSlave() {
      Wire.requestFrom(_address, sizeof(byte));
      while (Wire.available()) {
        _rxBuffer = Wire.read();
        Serial.println(_rxBuffer);
      }
    }

    bool scanBaseline() {
      //Write command for baseline scan
      //Wait 30s
      //Request 1 byte
      //if not good, wait 1s and re-request

    }
};
Slave scanner (SLAVE_ADDR);

void setup() {
  Wire.begin(MASTER_ADDR);        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  scanner.transmit(0b00000001);
  delay(20000);
  scanner.requestSlave();
  delay(100);
  scanner.transmit(0b00000010);
  delay(20000);
  scanner.requestSlave();
  delay(100);

}
