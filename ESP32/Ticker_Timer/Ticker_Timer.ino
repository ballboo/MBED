#include <Ticker.h>

Ticker  count_time1;
int h = 0, minn = 0, sec = 0, i = 0;

void c_time(){
  i++;
  if(i >= 10){
    sec++;
    i = 0; 
  }
  if(sec >= 60){
    minn++;
    sec = 0;
  }
  if(minn >= 60){
    h++;
    minn = 0;
  }
}

void setup() {
  Serial.begin(9600);
  count_time1.attach(0.1, c_time);     //Interrupt ทุกๆ 0.1 วินาที
}

void loop() {
 Serial.print(h); Serial.print(":");
 Serial.print(minn); Serial.print(":");
 Serial.print(sec); Serial.print(":");
 Serial.print(i); Serial.print(":");
 Serial.println();
 
}
