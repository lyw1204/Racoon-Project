class TrickArsenal{
  private:
    byte _pin1;
    byte _pin2;
    byte _pin3;
  public:
    TrickArsenal(byte dPIN1, byte dPIN2, byte dPIN3){
      _pin1 = dPIN1;
      _pin2 = dPIN2;
      _pin3 = dPIN3;

      pinMode(_pin1, OUTPUT);
      pinMode(_pin2, OUTPUT);
      pinMode(_pin3, OUTPUT);
      }
    void deploy(){
      int noTrigger = random(0,2);
      
      switch(noTrigger){//toggle outputs randomly for 1s
        case 0:
          digitalWrite(_pin2, HIGH);
          digitalWrite(_pin3, HIGH);
          delay(3000);
          digitalWrite(_pin2,LOW);
          digitalWrite(_pin3,LOW);
          break;
        case 1:
          digitalWrite(_pin1, HIGH);
          digitalWrite(_pin3, HIGH);
          delay(3000);
          digitalWrite(_pin1,LOW);
          digitalWrite(_pin3,LOW);        
          break;
        case 2:
          digitalWrite(_pin1, HIGH);
          digitalWrite(_pin2, HIGH);
          delay(3000);
          digitalWrite(_pin1,LOW);
          digitalWrite(_pin2,LOW);        
          break;
        default:
          break;

        }
      }

  };

  TrickArsenal ta1(DET1_PIN, DET2_PIN, DET3_PIN);
