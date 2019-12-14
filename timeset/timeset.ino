#include "Timealert.h"
int hour;
int minute;
int second;
boolean Run;
Timealert Times(1,0, 0);//set alarm
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  hour =  0; // set start hour
  minute = 0; // set start minute
  second = 0; // set start second
  
  xTaskCreate(
    TaskRuntime
    ,  "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  2 // Priority
    ,  NULL );

  xTaskCreate(
    Taskalert
    ,  "stepmotor"
    ,  128  // Stack size
    ,  NULL
    ,  1 // Priority
    ,  NULL );
}

void loop()
{

}

void TaskRuntime(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;)
  {
     second++;
    if (second>59){
      second=0;
      minute++;
      }
    else if (minute>59){
      minute=0;
      hour++;
    }
    else if (hour>24){
      hour=0;
    }
    Times.showtime(hour, minute, second);
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

void Taskalert(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  for (;;)
  {
    if (hour==Times.getHour()&& minute==Times.getMinute()){
      Times.alert();
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);  
  }
}
