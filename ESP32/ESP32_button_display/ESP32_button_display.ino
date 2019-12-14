//#include <Adafruit_GFX.h>    // Core graphics library
//#include <XTronical_ST7735.h> // Hardware-specific library
//#include <SPI.h>
//
//#define TFT_DC   2       // register select (stands for Data Control perhaps!)
//#define TFT_RST  4         // Display reset pin, you can also connect this to the ESP32 reset
//#define TFT_CS   5       // Display enable (Chip select), if not enabled will not talk on SPI bus
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);  


int button         = 4  ;
int StateCounter   = 0  ;   // 0 = Stop , 1 = Start...
int Sec            = 0  ;
int MiliSec        = 0  ;
int Relay          = 23 ;
int count          = 0  ;

hw_timer_t *timer = NULL;
void IRAM_ATTR onTimer(){
  MiliSec++;
  if(MiliSec == 10){
    MiliSec = 0;
    Sec++;
  }
   //SerialPrint();
   if (Sec == 5){
          Serial.println("Sec === 5");
          timerAlarmDisable(timer);
          Sec         = 0 ;
          MiliSec     = 0 ;
       } 

}

//************ Set Debouncing_time ************
long debouncing_time1 = 300; //Debouncing Time in Milliseconds
volatile unsigned long last_micros1;


void setup() {
  //tft.init(); 
  pinMode(button,INPUT_PULLUP);
  attachInterrupt(button,debounceInterrupt1,FALLING);

  Serial.begin(115200);
  pinMode(Relay, OUTPUT);

  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
  timerAlarmWrite(timer, 100000, true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmDisable(timer); 

  //tft.fillScreen(ST7735_BLACK);
//  tft.setCursor(0,0);
//  tft.setTextSize(2);
//  tft.setTextColor(ST7735_BLUE);
//  tft.println("ESP32");
 
  
}


void loop() {
    //DisplayPrint();
    SerialPrint();
}

void debounceInterrupt1() {
  if((long)(micros() - last_micros1) >= debouncing_time1 * 1000) {
    Handleinterrupt1();
    last_micros1 = micros();
  }
}

void Handleinterrupt1() {
  //Serial.println(StateCounter);
  if(StateCounter == 0){        
      StateCounter = 1;  
      //Serial.println("Lock"); 
    }
  else if(StateCounter == 1){
      timerAlarmEnable(timer);
      //Serial.println("UnLock");
      StateCounter = 0; 
     
    }
  }
   // Serial.println(StateCounter);
void SerialPrint(){
  if (count >= 100 ){
  Serial.print(" Sec :");
  Serial.print(Sec);
  Serial.println("");
  Serial.println(StateCounter);
  }
  else count += 1 ;
}
  

void DisplayPrint(){
  if (StateCounter == 0){
//      tft.setTextSize(2);
//      tft.setTextColor(ST7735_BLUE);
//      tft.println("Lock");
        Serial.println("Lock");
    }
    else{
//      tft.setTextSize(2);
//      tft.setTextColor(ST7735_BLUE);
//      tft.println("UnLock");
        Serial.println("UnLock");
    }
    delay(500);
}
