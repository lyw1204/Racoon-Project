//Pin Assignment

//IR sensor
int ledPin = D0;                // choose the pin for the LED
int inputPin = D5;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

//Ultrasonic sensor
int trigPin = D4;    // Trigger
int echoPin = D3;    // Echo
long duration, cm;


int ultrasonic_run(){ //Executes ultrasonic distance reading sequence
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 
  return cm;  
  }


void setup() {
  //IR Sensor
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input

  //Ultrasonic sensor
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}



void loop(){
  val = digitalRead(inputPin);  // read input value

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:


  if (val == HIGH)    // check if the input is HIGH
  {
    digitalWrite(ledPin, HIGH);  // turn LED ON

    if (pirState == LOW) 
    {
      Serial.println("Motion detected!");    // print on output change
      pirState = HIGH;
     
      Serial.print(ultrasonic_run());
      Serial.print("cm");
      Serial.println();
    }

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  delay(250);
  }
 
  else 
  {
    digitalWrite(ledPin, LOW); // turn LED OFF

    if (pirState == HIGH)
    {
      Serial.println("Motion ended!");    // print on output change
      pirState = LOW;
    }
  }
}
