#include <Wire.h>
#include <SPI.h>
#include <SH1106.h>
//#include "SH1106Ui.h"
//#include "images.h"

// Pin definitions for I2C
int OLED_SDA    = 21 ;  // pin 14
int OLED_SDC    = 22 ;  // pin 12
//int OLED_ADDR   = 0x3D ;

SH1106   display(0x3D, OLED_SDA, OLED_SDC);    // For I2C


void setup()   {                
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  display.init();
  display.setChar(0, 20,"Arial 16");


}
void loop() { 
  
}
