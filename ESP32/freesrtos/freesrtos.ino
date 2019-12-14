#define LED1 17 
#define LED2 2

void LEDTwo_Task(void *p){
  pinMode(LED2,OUTPUT);
  while(1){digitalWrite(LED2, !digitalRead(LED2));
  delay(500); 
  
  }
}

void LEDOne_Task(void *p){
  pinMode(LED1,OUTPUT);
  while(1){digitalWrite(LED2, !digitalRead(LED2));
  delay(500); 
  
  }
}

void setup(){
  xTaskCreate(&LEDTwo_Task, "LEDTwo_Task", 2048, NULL, 2, NULL);
  pinMode(LED2,OUTPUT);
  xTaskCreate(&LEDOne_Task, "LEDOne_Task", 2048, NULL, 1, NULL);
  pinMode(LED1,OUTPUT);
    
}
