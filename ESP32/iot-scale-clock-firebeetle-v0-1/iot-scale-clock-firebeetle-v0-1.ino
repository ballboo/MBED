// DIY Smart Scale with Alarm Clock v0.1 by Igor Fonseca @2018
// Based on Instructables Internet of Things Class sample code and SparkFun HX711 example
// Saves weight online on Adafruit.io.
// Displays measured weight on a local display.
// Read and display current time from an internet server.
// Configurable alarm clock with buzzer.
//
// Modified by Igor Fonseca 2018
// based on Instructables Internet of Things Class sample code by Becky Stern 2017
// based on Example using the SparkFun HX711 breakout board with a scale by Nathan Seidle 2017
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "XXXXX"
#define IO_KEY         "YYYYY"

/******************************* WIFI Configuration **************************************/

#define WIFI_SSID       "ZZZZZ"
#define WIFI_PASS       "WWWWW"

#include <AdafruitIO_WiFi.h>
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/

/******************************** Libraries **************************************/
#include <ESP8266WiFi.h> // ESP8266 library
#include <AdafruitIO.h>  // Adafruit IO library
#include <Adafruit_MQTT.h> // Adafruit MQTT library
#include <ArduinoHttpClient.h>  // http Client
#include <HX711.h>  // HX711 library for the scla
#include "DFRobot_HT1632C.h" // Library for DFRobot LED matrix display
#include <EEPROM.h> // Library for reading/writing on the memory

/******************************* Definitions *************************************/

#define calibration_factor -22795.0 //-7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT 0  // Pin connected to HX711 data output pin
#define CLK  12  // Pin connected to HX711 clk pin

#define NUM_MEASUREMENTS 10 // Number of measurements
#define THRESHOLD 2			// Measures only if the weight is greater than 2 kg. Convert this value to pounds if you're not using SI units.
#define THRESHOLD1 0.2  // Restart averaging if the weight changes more than 0.2 kg.

#define DATA D6 // Pin for DFRobot LED matrix display
#define CS D2   // Pin for DFRobot LED matrix display
#define WR D7   // Pin for DFRobot LED matrix display

#define MEM_ALOC_SIZE 8

DFRobot_HT1632C ht1632c = DFRobot_HT1632C(DATA, WR,CS); // set up LED matrix display

AdafruitIO_Feed *myWeight = io.feed("my-weight");     // set up the 'iot scale' feed
AdafruitIO_Feed *alarmTime = io.feed("alarm-clock");  // set up the 'iot scale' feed

String TimeDate = ""; // Variable used to store the data received from the server
int hours;    // Current hour
int minutes;  // Current minute
int seconds;  // Current second
int seconds0; // Last time the clock was updated
#define TIMEZONE  3 // Current time zone. Used for correction of server time

int alarmhour;    // Alarm hour
int alarmminute;  // Alarm minute
boolean alarm = false; // Alarm status. True = turn on the buzzer
boolean latch = false; // This variable inhibits the alarm until next minute

int bedtimehour = 23;      // Disables the display from this time until next morning
int bedtimeminute = 00;    // Disables the display from this time until next morning
boolean displayStatus = true;  // Display status. True = display on

HX711 scale(DOUT, CLK);  // Scale initialization
float weight = 0.0;      // Last measured weight
float prev_weight = 0.0; // Previous measeured weigt

int frequency = 600; // Sound frequency specified in Hz
#define buzzer  D3   // Buzzer positive pin
#define groundPin  D5 // Buzzer negative pin. It's used as a ground pin.
int timeOn = 200;     // Duration for buzzer on (specified in milliseconds)
int timeOff = 200;    // Duration for buzzer off (specified in millisecods)

/********************************** Setup ****************************************/

void setup() {

  // Configure IO pins
  pinMode(buzzer, OUTPUT);
  pinMode(groundPin, OUTPUT); // this pin was used to emulate a GND pin
  digitalWrite(groundPin, LOW);

  // Start the serial connection
  Serial.begin(115200);
  Serial.println("");
  Serial.println("***************************************************");
  Serial.println("DIY Smart Scale with Alarm Clock v0.1 by Igor @2018");
  Serial.println("***************************************************");
  
  // Setup LED matrix display
  ht1632c.begin();
  ht1632c.isLedOn(true);
  ht1632c.clearScreen();
  delay(500);
  
  // Connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // Set up a message handler for the 'my weight' feed.
  // The handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
    alarmTime->onMessage(handleMessage);

  // Wait for a connection
  ht1632c.print("connecting...",50);
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    ht1632c.print("...",50);
    ESP.wdtFeed();
    delay(500);
  }

  // We are connected
  Serial.println(io.statusText());
  ESP.wdtFeed();
  ht1632c.print("connected...",50);
  ht1632c.clearScreen();

  // Scale calibration
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  // Get current time from server
  getTime();
  seconds0 = millis();

  // Read alarm values from EEPROM memory
  EEPROM.begin(MEM_ALOC_SIZE);
  alarmhour = EEPROM.read(0);
  alarmminute = EEPROM.read(1);
  EEPROM.end();
  Serial.print("Alarm set to: ");
  if (alarmhour < 10) {
    Serial.print("0");
  }
  Serial.print(alarmhour);
  Serial.print(":");
  if (alarmminute < 10) {
    Serial.print("0");
  }
  Serial.println(alarmminute);
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run(); 
  ESP.wdtFeed();

  // Read current weight
  weight = scale.get_units();
  float avgweight = 0;
  if (weight > THRESHOLD) { // Takes measures if the weight is greater than the threshold
    float weight0 = scale.get_units();
    delay(1000);
	  for (int i = 0; i <= NUM_MEASUREMENTS; i++) {  // Takes several measurements
		  weight = scale.get_units();
      delay(100);
      Serial.println(weight);
		  avgweight = avgweight + weight;
      if (abs(weight - weight0) > THRESHOLD1) {
        avgweight = 0;
        i = 0;
      }
      weight0 = weight;
	  }
	  avgweight = avgweight / NUM_MEASUREMENTS; // Calculate average weight
	  Serial.print("Measured weight: ");
	  Serial.print(avgweight, 1);
	  Serial.println(" kg");
    
    alarm = false; // disable alarm
    latch = true;  // disable alarm for one minute
    
    char result[8]; // Buffer big enough for 7-character float
    dtostrf(avgweight, 6, 1, result);
    ht1632c.clearScreen();
	  ht1632c.print(result);  // Display measured weight on LED display
    
	  //save myWeight to Adafruit.io
	  myWeight->save(avgweight);
	  
	  // wait while there's someone on the scale
	  while (scale.get_units() > THRESHOLD) {
	    ESP.wdtFeed();
    }
	  
	  //keep LEDs on for two seconds then restart
	  delay(2000);
    ht1632c.clearScreen();
  }

  // update clock on display every 5 seconds
  if ((millis() - seconds0) > 5000) {
    seconds = seconds + 5;

    // synchronize with the server every 60 seconds
    if (seconds > 59) {
      getTime();
      latch = false;
    }

    // verify bed time
    if ((hours == bedtimehour) && (minutes == bedtimeminute)) {
      displayStatus = false; // disable display if it's already bed time
    }

    // display clock
    char clocktime[5];   // Buffer big enough for 5-character float
    char hourstime[1];   // Buffer big enough for 2-character float
    char minutestime[2]; // Buffer big enough for 2-character float

    // format clock value for the display
    if ((hours < 10) && (minutes < 10)) {
      sprintf(clocktime,"0%d:0%d",hours,minutes);  // Concatenate string for display
    }
    if ((hours < 10) && (minutes >= 10)) {
      sprintf(clocktime,"0%d:%d",hours,minutes);  // Concatenate string for display
    }
    if ((hours >= 10) && (minutes < 10)) {
      sprintf(clocktime,"%d:0%d",hours,minutes);  // Concatenate string for display
    }
    if ((hours >= 10) && (minutes >= 10)) {
      sprintf(clocktime,"%d:%d",hours,minutes);  // Concatenate string for display
    }
    ht1632c.clearScreen();
    if (displayStatus == true) {
      ht1632c.print(clocktime);  // Display current time on LED matrix
    }
    seconds0 = millis();
  }
  
  // trigger alarm clock
  if ((hours == alarmhour) && (minutes == alarmminute) && (latch == false)) {
      alarm = true;
      displayStatus = true; // turn on display
  }
  // activate buzzer
  if (alarm == true) {
    tone(buzzer, frequency);
    delay(timeOn);
    noTone(buzzer);
    delay(timeOff);
  }
}


// this function is called whenever a message
// is received from Adafruit IO. it was attached to
// the feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

    String commandStr = data->toString(); // store the incoming commands in a string
    
    // read alarm time
    alarmhour = commandStr.substring(0,2).toInt(); 
    alarmminute = commandStr.substring(3,5).toInt();
    
    // serial output received values
    Serial.print("Alarm set to: ");
    if (alarmhour < 10) {
      Serial.print("0");
    }
    Serial.print(alarmhour);
    Serial.print(":");
    if (alarmminute < 10) {
      Serial.print("0");
    }
    Serial.println(alarmminute);

    // store time on EEPROM memory
    EEPROM.begin(MEM_ALOC_SIZE);
    EEPROM.write(0,alarmhour);
    EEPROM.write(1,alarmminute);
    EEPROM.end();
}

//Get current time Google server
void getTime() {

  // connect server
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
    ESP.wdtFeed();
  }
  client.print("HEAD / HTTP/1.1\r\n\r\n");

  delay(500); //wait a few milisseconds for incoming message

  //if there is an incoming message
  if(client.available()){
    while(client.available()){
      if (client.read() == '\n') {    
        if (client.read() == 'D') {    
          if (client.read() == 'a') {    
            if (client.read() == 't') {    
              if (client.read() == 'e') {    
                if (client.read() == ':') {    
                  client.read();
                  String theDate = client.readStringUntil('\r');
                  client.stop();

                  TimeDate = theDate.substring(7);
                  String strCurrentHour = theDate.substring(17,19);
                  String strCurrentMinute = theDate.substring(20,22);
                  String strCurrentSecond = theDate.substring(23,25);
                  // store received data on global variables
                  hours = strCurrentHour.toInt();
                  hours = hours - TIMEZONE; // compensate for TIMEZONE
                  if (hours < 0) {
                    hours = hours + 24;
                  }
                  minutes = strCurrentMinute.toInt();
                  seconds = strCurrentSecond.toInt();
                }
              }
            }
          }
        }
      }
    }
  }
  //if no message was received (an issue with the Wi-Fi connection, for instance)
  else{
    seconds = 0;
    minutes += 1;
    if (minutes > 59) {
      minutes = 0;
      hours += 1;
      if (hours > 23) {
        hours = 0;
      }
    }
  }

  // serial output current time
  Serial.print("Current time: ");
  if (hours < 10) {
   Serial.print("0"); 
  }
  Serial.print(hours);
  Serial.print(":");
  if (minutes < 10) {
   Serial.print("0"); 
  }
  Serial.println(minutes);
  
}

