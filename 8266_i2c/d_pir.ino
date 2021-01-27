//PIR Interrupt Handling
void PIR_handler() {
  Serial.println("PIR TRIGGERED!");
  execStack.push(2);//Adds an scanNow event into execStack upon interrupt by PIR
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
