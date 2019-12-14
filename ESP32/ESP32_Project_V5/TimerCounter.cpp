#include "TimerCounter.h"
#include "Arduino.h"

hw_timer_t * timer = NULL;
int Count = 0;
int Sec = 0 ;

void IRAM_ATTR onTimer(){
    Count++;
    if(Count == 10){
    Count = 0;
    Sec++;
    
  }
    
}
TimerCounter::TimerCounter(){
   
}

void TimerCounter::SetTimer(int SelectTimer,int TimeInterval){
    timer = timerBegin(SelectTimer, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
    timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
    timerAlarmWrite(timer, TimeInterval, true); // 1000000 * 1 us = 1 s, autoreload true
    timerAlarmDisable(timer); // SetTimer Disable,Enable    
}

void TimerCounter::Start(){
    timerAlarmEnable(timer); // SetTimer Disable,Enable 
}

void TimerCounter::Stop(){
    timerAlarmDisable(timer); // SetTimer Disable,Enable 
}

void TimerCounter::Reset(){
     Count = 0 ;
     Sec = 0 ;
}

int TimerCounter::GetTime(){
    return Sec ;   
}
