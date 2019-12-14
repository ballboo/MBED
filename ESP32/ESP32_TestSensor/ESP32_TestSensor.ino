#include "Sensor.h"
//int buttonPin = 9;
//int ledPin =  4;
Sensor r1(2) ;


//int buttonState = 0;

void setup() {



  Serial.begin(115200);
  //pinMode(ledPin, OUTPUT);
  //pinMode(buttonPin, INPUT);

}

void loop() {
  r1.CheckStatus(r1.GetSensor()) ;
  //Serial.println(r1.GetSensor());
  // read the state of the pushbutton value:

  //buttonState = digitalRead(buttonPin);



//  if (buttonState == HIGH) {
//    // turn buzzer on:
//    Serial.println("turn buzzer on");
//    digitalWrite(ledPin, HIGH);
//
//  }
//  else {
//    // turn buzzer off:
//    Serial.println("turn buzzer off");
//    digitalWrite(ledPin, LOW);
//
//  }


}
