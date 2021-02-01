//PIR Interrupt Handling
ICACHE_RAM_ATTR void PIR_handler() {
  Serial.println("PIR TRIGGERED!");
  execStack.push(2);//Adds an scanNow event into execStack upon interrupt by PIR
  pirFlag = true; 
}

class PIR {
  private:
    int _pin;
    bool _pirState = LOW;

  public:
    PIR(byte PIN) {
      _pin = PIN;
      pinMode(_pin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(_pin), PIR_handler, RISING);
      noInterrupts();
    }

    void enable() {
      interrupts();
    }

    void disable() {
      noInterrupts();
    }
};

PIR pir1(PIR_L);//PIR Initialized
//PIR pir2 = PIR_R;
