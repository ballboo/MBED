#include <WiFi.h>

const char* ssid     = "BBL";
const char* password = "12345678";

WiFiServer server(80);

String header;

String output02State = "off";
int readSenor  ; 
const int output02 = 02;
int MiliSec           = 0 ;
int Sec               = 0 ;

hw_timer_t * timer = NULL;
void IRAM_ATTR onTimer(){
   MiliSec++;
  if(MiliSec == 10){
    MiliSec = 0;
    Sec++;
  }
  Serial.println(Sec);

}

void setup(){
  Serial.begin(115200);

  pinMode(output02, OUTPUT);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();


  timer = timerBegin(0, 80, true);  // timer 0, MWDT clock period = 12.5 ns * TIMGn_Tx_WDT_CLK_PRESCALE -> 12.5 ns * 80 -> 1000 ns = 1 us, countUp
  timerAttachInterrupt(timer, &onTimer, true); // edge (not level) triggered 
  timerAlarmWrite(timer, 100000, true); // 1000000 * 1 us = 1 s, autoreload true
  timerAlarmDisable(timer); // SetTimer Disable,Enable    
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /02/on") >= 0) {
              Serial.println("GPIO 02 on");
              output02State = "Lock";
              digitalWrite(output02, HIGH);
            } else if (header.indexOf("GET /02/off") >= 0) {
              Serial.println("GPIO 02 off");
              output02State = "UnLock";
              digitalWrite(output02, LOW);
              //timerAlarmEnable(timer);
            } 
//            if(Sec==5 && output02State=="UnLock" ){
//                client.println("<p><a href=\"/02/on\"><button>Lock</button></a></p>");
//                timerAlarmDisable(timer);
//                Sec = 0 ;
//              }
     
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #f44336; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart Door by 3B</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>State " + output02State + "</p>");
            // If the output26State is off, it displays the ON button       
            if (output02State=="UnLock") {
              client.println("<p><a href=\"/02/on\"><button>Lock</button></a></p>");
            } else {
              client.println("<p><a href=\"/02/off\"><button>UnLock</button></a></p>");
            } 
                 
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
