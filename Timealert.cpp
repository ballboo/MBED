#include "Arduino.h"
#include "Timealert.h"

Timealert::Timealert(int Hour, int Minute, int Second){
    Serial.print("SetHour =");
    Serial.println(Hour);
    Serial.print("SetMinute =");
    Serial.println(Minute);
    Serial.print("SetSecond =");
    Serial.println(Second);
    H = Hour;
    M = Minute;
    S = Second;
}
void Timealert::showtime(int hour, int minute, int second){
    Serial.print("H:");
    if(hour < 10){
      Serial.print("0");}
    Serial.print(hour);
    Serial.print(" M:");
    if(minute < 10){
      Serial.print("0");}
    Serial.print(minute);
    Serial.print(" S:");
    if(second < 10){
      Serial.print("0");}
    Serial.println(second);
}
void Timealert::alert(){
    Serial.println("alert alert alert alert");
}

int Timealert::getHour(){
  return H;
}
int Timealert::getMinute(){
  return M;
}
int Timealert::getSecond(){
  return S;
}
