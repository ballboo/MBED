#include "MicroGear.h"
#include "WiFi.h"
#include "TimerCounter.h"
#include "Sensor.h"

const char* ssid     = "BBL";
const char* password = "12345678";
//const char* ssid     = "iot-lab";
//const char* password = "computing";
//const char* ssid     = "Ballboo088's iPhone";
//const char* password = "password";

#define APPID   "SmartDoorProject"
#define KEY     "8Bym7tve4qiCd56"
#define SECRET  "N6VQcrycyUo8Ucepq2jsONUol"
#define ALIAS   "ESP32"
#define FREEBORADALIAS    "SmartDoor"               //ชื่อ Device Alias จาก NETPIE Freeboard

xTaskHandle ReadSensor_Task_Handle;
xTaskHandle Webserver_Task_Handle;

#define button 4  
//#define Sensor 2   
char str[32];
unsigned long lastTime = 0;

int StateCounter   = 0  ;   // 0 = Stop , 1 = Start...
int Relay          = 23 ;

TimerCounter t1;
Sensor r1(2) ;


WiFiClient client;
int timer1 = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++)
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }

  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

//เช็คคำสั่งจากที่ได้รับจาก NETPIE Freeboard
  if (stateStr == "Lock")
  {
    digitalWrite(button, LOW);
    microgear.chat(FREEBORADALIAS, "Lock");        //==== คำสั่ง chat เพื่อบอกส่งค่าสถานะ ON กลับไปยังหลอด LED บน NETPIE Freeboard
    Serial.println("Lock");
  }
  else if (stateStr == "UnLock")
  {
    digitalWrite(button, HIGH);
    microgear.chat(FREEBORADALIAS, "UnLock");       //==== คำสั่ง chat เพื่อบอกส่งค่าสถานะ OFF กลับไปยังหลอด LED บน NETPIE Freeboard
    Serial.println("UnLock");
    t1.Start();
  }
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}


void setup() {

  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(button, OUTPUT);
  pinMode(Relay, OUTPUT);
  t1.SetTimer(0,100000); 
  xTaskCreate(&ReadSensorSwitch_Task, "ReadSensorSwitch_Task", 1024, NULL, 1, &ReadSensor_Task_Handle);
  xTaskCreate(&Webserver_Task, "Webserver_Task", 20480, NULL, 2, &Webserver_Task_Handle);

  
  
}
void loop() {
     
  }

void ReadSensorSwitch_Task(void *p){
    Serial.println("Start ReadSensorSwitch_Task");
    while(1){
//          Serial.print("ReadSensorSwitch -->");
//          Serial.println(r1.GetSensor());
          r1.CheckStatus(r1.GetSensor()) ;

          vTaskDelay(1000);
      }

  
}
void Webserver_Task(void *p){
  Serial.println("Start Webserver_Task");

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);

   while(1){
    Serial.println(t1.GetTime());
    if (t1.GetTime() == 7 ){
      t1.Reset();
      }
    if (t1.GetTime() == 5 && r1.GetSensor() == 0){
      digitalWrite(button, LOW);
      Serial.println("Lock");
      t1.Stop();
      t1.Reset();
    }
    if (microgear.connected()) {
    microgear.loop();
     timer1 = 0;
        if(millis()-lastTime > 1000){
          // แปลงค่าข้อมูลให้เป็นชนิด char สำหรับส่งข้อมูลผ่าน NETPIE
          sprintf(str,"%d",r1.GetSensor());
          Serial.print("Sending --> ");
          Serial.println(str);
          microgear.publish("/sensor",str);
          vTaskDelay(1000);
      }
       else {
    Serial.println("connection lost, reconnect...");
    if (timer1 >= 5000) {
            microgear.connect(APPID); 
            timer1 = 0;
        }
        else timer1 += 100;
        delay(100);
    }
      
    }
        vTaskDelay(100);
    }
  }
 
