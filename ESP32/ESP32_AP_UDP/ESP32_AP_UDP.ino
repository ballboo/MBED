#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "AsyncUDP.h"

IPAddress local_ip = {192,168,1,1};       // สำหรับ AP Mode        
IPAddress gateway = {192,168,1,1};
IPAddress subnet = {255,255,255,0};


const char *ssid = "BALL";
const char *password = "12345678";

WiFiServer server(80);
AsyncUDP udp;


void setup() {
Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip,gateway,subnet);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  
  if(udp.listen(1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", To: ");
            Serial.print(packet.localIP());
            Serial.print(":");
            Serial.print(packet.localPort());
            Serial.print(", Length: ");
            Serial.print(packet.length());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
    }
}


void loop() {
  delay(1000);
    //Send broadcast
    udp.broadcast("Anyone here?");

}
