#include "StopWatch.h"
#include "Arduino.h"
//************ Set Time ************
int Hour,Min,Sec,MiliSec     ;

hw_timer_t * timer = NULL;

void IRAM_ATTR onTimer(){
MiliSec++;
  if(MiliSec == 100){
    MiliSec = 0;
    Sec++;
    if(Sec == 60){
      Sec = 0;
      Min++;
      if(Min == 60){
        Min = 0;
        Hour++;
        if(Hour == 99){
          Hour = 0;
        }
      }
    }
  }
}
StopWatch::StopWatch(){
    timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
    timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
    timerAlarmWrite(timer, 100000, true); // 1000000 * 1 us = 1 s, autoreload true
    timerAlarmDisable(timer); // SetTimer Disable,Enable    
}

void StopWatch::SetPin(int Pin_Start_Stop){
    pinMode(Pin_Start_Stop,INPUT_PULLUP);
}

void StopWatch::Start(){
    timerAlarmEnable(timer); // SetTimer Disable,Enable 
}

void StopWatch::Stop(){
    timerAlarmDisable(timer); // SetTimer Disable,Enable 
}

int StopWatch::GetHour(){
    return Hour ;   
}
int StopWatch::GetMin(){
    return Min ;   
}
int StopWatch::GetSec(){
    return Sec ;   
}
int StopWatch::GetMiliSec(){
    return MiliSec ;   
}

void StopWatch::SerialPrint(){
  Serial.print("H :");
  Serial.print(Hour);
  Serial.print(" M :");
  Serial.print(Min);
  Serial.print(" S :");
  Serial.print(Sec);
  Serial.print(" MS :");
  Serial.print(MiliSec);
  Serial.println("");
  }