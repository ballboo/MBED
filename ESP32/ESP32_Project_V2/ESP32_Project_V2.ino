#include "MicroGear.h"
#include "WiFi.h"

const char* ssid     = "BBL";
const char* password = "12345678";

#define APPID   "SmartDoorProject"
#define KEY     "8Bym7tve4qiCd56"
#define SECRET  "N6VQcrycyUo8Ucepq2jsONUol"
#define ALIAS   "ESP32"
#define FREEBORADALIAS    "SmartDoor"               //ชื่อ Device Alias จาก NETPIE Freeboard


#define button 4  
#define Sensor 2   
char str[32];
unsigned long lastTimeDHT = 0;

int StateCounter   = 0  ;   // 0 = Stop , 1 = Start...
int Sec            = 0  ;
int MiliSec        = 0  ;
int Relay          = 23 ;
int count          = 0  ;
int ReadSensor     = 0  ;
//int lastReadSensor = 0  ;

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
          Serial.println("Lock");
          digitalWrite(button, LOW);
       } 

}

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
    timerAlarmEnable(timer);
    Serial.println("UnLock");

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

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(button, OUTPUT);
  pinMode(Sensor, INPUT);


  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);
  pinMode(ReadSensor, INPUT);
  pinMode(Relay, OUTPUT);
  timer = timerBegin(0, 80, true);  
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
  timerAlarmWrite(timer, 100000, true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmDisable(timer); 


}

void loop() {
   ReadSensor = digitalRead(Sensor);

  delay(1000);
  
  if (microgear.connected()) {
    //Serial.println("connected");
    microgear.loop();
     timer1 = 0;
        if(millis()-lastTimeDHT > 1000){
          lastTimeDHT = millis();
          ReadSensor = digitalRead(Sensor);     // อ่านค่าความชื้น          
             if (ReadSensor == HIGH){
                Serial.println("Door Open");

              }

            else {
                Serial.println("Door Close");
              }
          
          // แปลงค่าข้อมูลให้เป็นชนิด char สำหรับส่งข้อมูลผ่าน NETPIE
          sprintf(str,"%d",ReadSensor);
          
          Serial.print("Sending --> ");
          Serial.println(str);
          microgear.publish("/sensor",str);
  

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
}
