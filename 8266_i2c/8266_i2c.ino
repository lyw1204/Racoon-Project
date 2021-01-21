#include <common.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <Utils.h>

#include <ESP8266WiFi.h> 
#include <Wire.h>

#define SCL D1
#define SDA D2

#define PIR_L D5
#define PIR_R D6

#define SPEAKER D8

#define MASTER_ADDR 1
#define SLAVE_ADDR 8

#define FIREBASE_HOST "https://raccoonlock-default-rtdb.firebaseio.com/" //Without http:// or https:// schemes
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"
#define WIFI_SSID "DAVID NEW"
#define WIFI_PASSWORD "c=3.00*10^8m/s"





//I2C communication
class Slave {
  private:
    uint8_t _address;
    uint8_t _rxBuffer;
    uint8_t _txBuffer;
    byte _healthState;

    bool transmit(byte command) {
      Wire.beginTransmission(_address);
      Wire.write(command);
      Wire.endTransmission();
      _txBuffer = command;
    }

    byte requestSlave() {
      Wire.requestFrom(_address, sizeof(byte));
      byte rx;
      while (Wire.available()) {
        rx = Wire.read();
      }
      return rx;
    }
  public:
    Slave(unsigned short address) {
      _address = address;
    }

    bool getBaseline() {
      Serial.println("getting baseline now");

      transmit(1);
      delay(50000);//Wait 50s for scan to complete, should change to nonblocking in future
      byte result = requestSlave();
      if (result == 0) {
        return true;
      }
      else {
        return false;
      }
    }
    bool getDepthNow() {
      Serial.println("Scanning now");
      transmit(2); //Tell arduino to scanNow over I2C
      delay(50000);//Wait 50s for scan to complete, should change to nonblocking in future
      byte result = requestSlave();
      if (result == 1) {
        return true;
      }
      else {
        return false;
      }
    }

    void alarm() {
      transmit(3);
      delay(5000);
      requestSlave();

    }


    void selfTest() {
      transmit(4);
      delay(500);
      _healthState = requestSlave();
      //_healthState = 0b00001111;
      Serial.print("Health Test:");
      Serial.println(_healthState);

      if (_healthState != 0b00000000) {
        while (true) {
          errorTones();
        }

      }
    }

    void errorTones() {
      byte tempHealth = _healthState;
      byte mask = 0b00000001;

      for (int i = 0; i < 8; i++) {
        if (tempHealth & mask) { //rightmost bit HIGH
          tone(SPEAKER, 2000);

        }
        else { //rightmost bit low
          tone(SPEAKER, 700);
        }
        delay(500);
        noTone(SPEAKER);
        delay(500);
        tempHealth >>= 1; //shift health bit left
      }

      delay(2000);
    }

};
Slave scanner (SLAVE_ADDR);


//PIR Interrupt Handling
void PIR_handler() {
  Serial.println("PIR TRIGGERED!");


}

class PIR {
  private:
    byte _pin;
    bool _pirState = LOW;

  public:
    PIR(byte PIN) {
      _pin = PIN;
      pinMode(_pin, INPUT);

    }

    void enable() {
      attachInterrupt(digitalPinToInterrupt(_pin), PIR_handler, RISING);
    }

    void disable() {
      detachInterrupt(digitalPinToInterrupt(_pin));
    }


};

PIR pir1 = (PIR_L);//Left PIR Initialized


//WIFI and DB section

char ssid [32];
char password [63];
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config; 

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID); Serial.println(" ...");

  int teller = 0;
  while (WiFi.status() != WL_CONNECTED)
  {                                       // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++teller); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

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
