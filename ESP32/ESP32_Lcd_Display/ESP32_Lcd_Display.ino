#include <Adafruit_GFX.h>    // Core graphics library
#include <XTronical_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define TFT_DC     2       // register select (stands for Data Control perhaps!)
#define TFT_RST   4         // Display reset pin, you can also connect this to the ESP32 reset
#define TFT_CS   5       // Display enable (Chip select), if not enabled will not talk on SPI bus
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);  

void setup() {
  tft.init(); 
}

void loop() {
  tftPrintTest() ;
}
void tftPrintTest() {
  tft.setTextWrap(false);
  //tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("I'm running");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("on a");
  tft.setTextColor(ST7735_BLUE);
  tft.println("ESP32");
  tft.setTextSize(4);
  tft.setTextColor(ST7735_CYAN);
  tft.print("Ha ha!");
}
