#include "Sensor.h"
#include "Arduino.h"

int ReadSensor1 = 0 ;
Sensor::Sensor(int pin){
   pinMode(pin, INPUT);
   _pin = pin ; 
}

int Sensor::GetSensor(){
      ReadSensor1 = digitalRead(_pin);

      return ReadSensor1 ;
}

void Sensor::CheckStatus(int num){
            if (num == HIGH){
                Serial.println("Door Open");
              }
            else {
                Serial.println("Door Close");
              }
}

