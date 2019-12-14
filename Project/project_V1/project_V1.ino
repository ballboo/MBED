#include <WiFi.h> 
#include <analogWrite.h>

#define MOTOR_A_PWM_Pin 14 
#define MOTOR_A_DIR_Pin 5
#define MOTOR_B_PWM_Pin 15
#define MOTOR_B_DIR_Pin 4

const char* ssid = "BBL" ;
const char* password = "12345678" ;

int forwardSpeed = 950 ;
int turnSpeed = 600 ;
int forwardDir = LOW ; 
int backwardDir = HIGH ;

WiFiServer server(80); 

void setup(){
    Serial.begin(115200);
    delay(10);

    Serial.println();
    Serial.print("Connecting to");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");

    }
    server.begin();
    Serial.println("Server Started");

    Serial.println(WiFi.localIP());

    pinMode(MOTOR_A_PWM_Pin, OUTPUT);
    pinMode(MOTOR_A_DIR_Pin, OUTPUT);
    pinMode(MOTOR_B_PWM_Pin, OUTPUT);
    pinMode(MOTOR_B_DIR_Pin, OUTPUT);

    stopCar();
    
}
void loop(){
    WiFiClient client = server.available();
    if (!client){
        return;
    }

    Serial.println("\nnew client");
    while(!client.available()){
        delay(20); 
    }

    String req = client.readStringUntil('\n');
    Serial.print("REQ Wfrom client <-") ;
    Serial.print(req);
    client.flush();

    if(req.indexOf("/index.html")!= -1 ){
        client.println(getHtmlPage());
        return;
    }
    else if (req.indexOf("/engines/") != -1){
       String parameters = req.substring(13);
       int separatorPos = parameters.indexOf(",");
       int httpPos = parameters.indexOf(" HTTP");

       String dirChar = parameters.substring(separatorPos + 1, httpPos);
       Serial.println("CharDir" + dirChar); 
       
       if (dirChar.equals("f")){
           outputMotor(forwardSpeed, forwardSpeed, forwardDir);
       }
       else if (dirChar.equals("l")){
           outputMotor(turnSpeed, forwardSpeed, forwardDir);
       }
       else if (dirChar.equals("r")){
           outputMotor(forwardSpeed, turnSpeed, forwardDir);
       }
       else if (dirChar.equals("b")){
           outputMotor(1023-turnSpeed, 1023-turnSpeed, backwardDir);
       }
       else if(dirChar.equals("s")){
           stopCar();
       }
   }
    
    client.flush();

    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" ;
    s += "<!DOCTYPE HTML>\r\n<html>\r\n Car";
    s += "</html>\n" ;

    client.print(s) ; 
    delay(1);
    //client.println(getHtmlPage());

    Serial.println("Client disonnected");
    delay(100);

}
void outputMotor(int speed_A,int speed_B,int mDir){
    analogWrite(MOTOR_A_PWM_Pin, speed_A);
    digitalWrite(MOTOR_A_DIR_Pin, mDir);

    analogWrite(MOTOR_B_PWM_Pin, speed_B);
    digitalWrite(MOTOR_B_DIR_Pin, mDir);

    Serial.printf("Motor A %d dir %d, B %d Dir %d \n", speed_A, mDir, speed_B, mDir);
}

void stopCar(){
    analogWrite(MOTOR_A_PWM_Pin,0);
    digitalWrite(MOTOR_A_DIR_Pin,0);

    analogWrite(MOTOR_B_PWM_Pin,0);
    digitalWrite(MOTOR_B_DIR_Pin,0);
    
}
String getHtmlPage() {
    String js = createJS();
    String css = "<style>.button {background-color: #990033;border: none;border-radius: 4px;color: white;padding: 7px 15px;font-size: 20px;text-aglien: center;text-decoration: none;text-decoration: none;}</style>" ;
    
    String strHtml = "<!DOCTYPE html><html><head>" ;
    strHtml += js ;
    strHtml += css ; 
    strHtml += "<meta http-equiv='Content-Type' content='text/html; charset=UTP-8'>";
    strHtml += "<meta name=\"viewport\" content=\"initial-scale=1, maximun-scale=2>";
    strHtml += "<title>Controller</title>";
    strHtml += "</head><body onload='init()'><center><h1>IoT Car</h1>";
    strHtml += "<br>";
    strHtml += "<input type='button' class='button' id='btnF' value='Forward'><br><br>";
    strHtml += "<input type='button' class='button' id='btnL' value='<< Left'>&nbsp; ";
    strHtml += "&nbsp; <input type='button' class='button' id='btnR' value='Right >>'><br><br> ";
    strHtml += "<input type='button' class='button' id='btnR' value='Back'><br><br> ";
    strHtml += "<div id='/status'/>..</div>";
    strHtml +=  "</center></body></html>";

    return strHtml ;
}

String createJS(){
    String js = "<script type='text/javascipt'>";
    js += "fucntion init(){";
    js += "    document.getElementByld('btnF').addEventListener('touchstart', moveF ,false);";
    js += "    document.getElementByld('btnF').addEventListener('touchend', stopCar ,false);";
    js += "    document.getElementByld('btnL').addEventListener('touchstart', moveL ,false);";
    js += "    document.getElementByld('btnL').addEventListener('touchend', stopCar ,false);";
    js += "    document.getElementByld('btnR').addEventListener('touchstart', moveR ,false);";
    js += "    document.getElementByld('btnR').addEventListener('touchend', stopCar ,false);";
    js += "    document.getElementByld('btnB').addEventListener('touchstart', moveB ,false);";
    js += "    document.getElementByld('btnB').addEventListener('touchend', stopCar ,false);";
    js += "}    ";
   

    js += "fucntion move(dir){";
    js += "     document.getElementByld('status').innerHTML = dir;";
    js += "     var request = new XMLHttpRequest();";
    js += "     request.open('GET', '/engines/'+ dir , true);";
    js += "     request.send(null);";

    js += "function moveF(){";
    js += "     move('f'); ";
    js += "}    ";

    js += "function moveL(){";
    js += "     move('l'); ";
    js += "}    ";

    js += "function moveR(){";
    js += "     move('r'); ";
    js += "}    ";

    js += "function moveB(){";
    js += "     move('b'); ";
    js += "}    ";

    js += "function stopCar(){" ;
    js += "     document.getElementByld('status').innerHTML = 's';";
    js += "     var request = new XMLHttpRequest() ;";
    js += "     request.open('GET', '/engines/s'+ dir , true);";
    js += "     request.send(null);";
    js += "}    ";
    js +="</script>";
    //Serial.println(js) ;
    return(js);
}
