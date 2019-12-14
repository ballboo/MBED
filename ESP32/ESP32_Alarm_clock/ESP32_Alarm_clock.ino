#include <WiFi.h>
#include "time.h"

const char* ssid       = "BBL";
const char* password   = "12345678";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 21600;

int hr1 = 11 ; 
int minut1 = 13 ; 
int hour_1 ;
int min_1 ;
int sec_1 ; 


void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  hour_1 = timeinfo.tm_hour ;
  min_1 = timeinfo.tm_min ;
  sec_1 = timeinfo.tm_sec ;

  Serial.print("Time:");
  //Serial.print();
  Serial.print(hour_1);
  Serial.print(":");
  Serial.print(min_1);
  Serial.print(":");
  Serial.print(sec_1);
  Serial.print("\n");



}

void setup()
{
  Serial.begin(115200);

  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
  alarm();
}


void alarm ()
{
  Serial.print("Alarm Time:");
  Serial.print(hr1);
  Serial.print(':');
  Serial.print(minut1);
  Serial.print("\n");


  if (hour_1 == hr1 && min_1 == minut1){ //Comparing the current time with the Alarm time

    //Buzzer();
    //Buzzer();
    Serial.print("Alarm ON");
    Serial.print("Alarming");
    //Buzzer();
   // Buzzer();


  }
}

//void Buzzer()
//{
//  digitalWrite(buzzer, HIGH);
//  delay(500);
//  digitalWrite(buzzer, LOW);
//  delay(500);
//}
