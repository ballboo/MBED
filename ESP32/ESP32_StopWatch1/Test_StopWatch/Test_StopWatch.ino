#include <StopWatch.h>
StopWatch stop_1;

int interruptPin1     = 0;   
bool StateTimer        = false;    // false = Stop , true = Start...

long debouncing_time1 = 300; 
volatile unsigned long last_micros1;

void setup() {
  stop_1.SetPin(interruptPin1);                        
  attachInterrupt(interruptPin1,debounceInterrupt1,FALLING);  // Set InrerruptPin1 
  Serial.begin(9600);
  Serial.println("Test StopWacth ESP32 ");     
  showtimer();
}

void debounceInterrupt1() {
  if((long)(micros() - last_micros1) >= debouncing_time1 * 1000) {
    Handleinterrupt1();
    last_micros1 = micros();
  }
}

void Handleinterrupt1() {
  if(StateTimer == false){            // if Timer is Stop
    stop_1.Start();                       // Enable Timer
    StateTimer = true ;        // Timer is Start
    }
  else{
    stop_1.Stop();                        // Disable Timer
    StateTimer = false;
    }
}

void loop() {

}
void showtimer(){
  while(1){
       stop_1.SerialPrint();
       
  }                                  
}
