#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>

const char* ssid     = "BBL";      //ใส่ชื่อเครือข่าย WiFi ที่จะให้ Espresso Lite v2.0 เชื่อมต่อ
const char* password = "12345678";            //ใส่รหัสผ่านของเครือข่าย WiFi ที่จะให้ Espresso Lite v2.0 เชื่อมต่อ


#define LEDPin 15                                 //หากต่อหลอด LED ที่ขาอื่นที่ไม่ใช่ 15 ให้ทำการเปลี่ยนชื่อขาตรงนี้ด้วย
#define APPID       "SmartDoorProject"              //ใส่ Application ID (ชื่อ Application) ที่สร้างบน NETPIE
#define KEY     "byBtdBMuHERgw45"             //ใส่ Key ของอุปกรณ์ ที่ได้จากการสร้างบน NETPIE
#define SECRET  "TKJaO1pBlqPUGHAQhrYxTvb4r"   //ใส่ Secret ของอุปกรณ์ ที่ได้จากการสร้างบน NETPIE
#define ALIAS       "lednode5555"         //ตั้งชื่อ Alias ที่ได้จากการสร้างบน NETPIE ให้กับ Things นี้ (เป็นชื่อเรียกเวลา chat มาหา)
#define FREEBORADALIAS       "free_ledtest"               //ชื่อ Device Alias จาก NETPIE Freeboard

WiFiClient client;
//AuthClient *authclient;

MicroGear microgear(client);

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen)
{
  Serial.println("Connected to NETPIE...");
  microgear.setName(ALIAS);
}

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen)
{
  //=======รับ chat มาจาก NETPIE freeboard===========
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
  if (stateStr == "ON")
  {
    digitalWrite(LEDPin, HIGH);
    microgear.chat(FREEBORADALIAS, "ON");        //==== คำสั่ง chat เพื่อบอกส่งค่าสถานะ ON กลับไปยังหลอด LED บน NETPIE Freeboard
  }
  else if (stateStr == "OFF")
  {
    digitalWrite(LEDPin, LOW);
    microgear.chat(FREEBORADALIAS, "OFF");       //==== คำสั่ง chat เพื่อบอกส่งค่าสถานะ OFF กลับไปยังหลอด LED บน NETPIE Freeboard
  }

}

void setup()
{
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(LEDPin, OUTPUT);

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);

  if (WiFi.begin(ssid, password)) //เริ่มเชื่อมต่อ wifi
  {

    while (WiFi.status() != WL_CONNECTED) //ถ้ายังเชื่อม wifi ไม่ติดจะปริ้นจุดไปเรื่อยๆ
    {
      delay(500);
      Serial.print(".");
    }
    //ต่อ wifi ได้แล้ว
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  //uncomment the line below if you want to reset token -->
  //microgear.resetToken();
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

}


void loop()
{
  if (microgear.connected()) //ยังคง connect NETPIE ได้
  {
    microgear.loop();
    Serial.println("connect...");
  }
  else
  {
    Serial.println("connection lost, reconnect..."); //NETPIE ขาดการเชื่อมต่อ ให้เชื่อมต่อ NETPIE อีกครั้ง
    microgear.connect(APPID);
  }
  delay(1000);
}
