#include "MicroGear.h"
#include "WiFi.h"

const char* ssid     = "BBL";
const char* password = "12345678";

#define APPID   "SmartDoorProject"
#define KEY     "8Bym7tve4qiCd56"
#define SECRET  "N6VQcrycyUo8Ucepq2jsONUol"
#define ALIAS   "ESP32"
#define FREEBORADALIAS    "SmartDoor"               //ชื่อ Device Alias จาก NETPIE Freeboard


#define LEDPin 2                                 //หากต่อหลอด LED ที่ขาอื่นที่ไม่ใช่ 15 ให้ทำการเปลี่ยนชื่อขาตรงนี้ด้วย

WiFiClient client;
int timer = 0;
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

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.setAlias(ALIAS);
}


void setup() {

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(LEDPin, OUTPUT);


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
}

void loop() {
  if (microgear.connected()) {
    //Serial.println("connected");
    microgear.loop();
  
  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);

  delay(100);
  }
}
