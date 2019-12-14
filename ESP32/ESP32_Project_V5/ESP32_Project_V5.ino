#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "TimerCounter.h"
#include "Sensor.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "iot-lab"
#define WLAN_PASS       "computing"
//#define WLAN_SSID       "Ballboo088's iPhone"
//#define WLAN_PASS       "password"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ballboo088"
#define AIO_KEY         "5f160353be4b446a8b4c592f7258ee03"

xTaskHandle ReadSensor_Task_Handle;
xTaskHandle Webserver_Task_Handle;


#define button 4  
char str[32];
int StateCounter   = 0  ;
int Relay          = 23 ;

TimerCounter t1;
Sensor r1(2) ;
int timer1 = 0;


/************ Global State (you don't need to change this!) ******************/
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/button");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting...");
  pinMode(button, OUTPUT);
  pinMode(Relay, OUTPUT);
  t1.SetTimer(0,100000); 
  xTaskCreate(&ReadSensorSwitch_Task, "ReadSensorSwitch_Task", 1024, NULL, 1, &ReadSensor_Task_Handle);
  xTaskCreate(&Webserver_Task, "Webserver_Task", 20480, NULL, 2, &Webserver_Task_Handle);
  // Connect to WiFi access point.
  
}

uint32_t x=0;

void loop() {


}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
void Webserver_Task(void *p){
  Serial.println("Start Webserver_Task");
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&onoffbutton);

  while(1){
   if (t1.GetTime() >= 7 ){
      t1.Reset();
      }
  //Serial.println(t1.GetTime());
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      String stateStr = (char *)onoffbutton.lastread ; 
      if (stateStr == "Lock"){
        digitalWrite(button, LOW);
        Serial.println("Lock");
      }
      else if (stateStr == "UnLock"){
        digitalWrite(button, HIGH);
        Serial.println("UnLock");
        t1.Start();
      }
    }
    }
    if (t1.GetTime() == 5 && r1.GetSensor() == 0){
      digitalWrite(button, LOW);
      Serial.println("Lock");
      t1.Stop();
      t1.Reset();
    }
    Serial.print("ReadSensorSwitch -->");
    Serial.println(r1.GetSensor());
    if (r1.GetSensor() == 0){
        photocell.publish("Door Close");
      }
    else if (r1.GetSensor() == 1){
        photocell.publish("Door Open");
      }
    }
   
   vTaskDelay(100);
  }

void ReadSensorSwitch_Task(void *p){
    Serial.println("Start ReadSensorSwitch_Task");
    while(1){
          r1.CheckStatus(r1.GetSensor()) ;

          vTaskDelay(1000);
      }

  
}
