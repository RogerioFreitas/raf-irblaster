/*
 * IRremoteESP8266: IRServer - demonstrates sending IR codes controlled from a webserver
 * An IR LED must be connected to ESP8266 pin 0.
 * Version 0.1 June, 2015
 */
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>

const char* ssid = "Full4it";
const char* password = "2505116928roger";
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(3);

void handleRoot() {
 server.send(200, "text/html", "<html><head> <title>ESP8266 Demo</title></head><body><h1>Hello from ESP8266, you can send NEC encoded IR signals from here!</h1><p><a href=\"ir?code=16769055\">Send 0xFFE01F</a></p><p><a href=\"ir?code=16429347\">Send 0xFAB123</a></p><p><a href=\"ir?code=16771222\">Send 0xFFE896</a></p></body></html>");
}

//+=============================================================================
// Convert string to hex, borrowed from ESPBasic
//
unsigned long HexToLongInt(String h)
{
  // this function replace the strtol as this function is not able to handle hex numbers greather than 7fffffff
  // I'll take char by char converting from hex to char then shifting 4 bits at the time
  int i;
  unsigned long tmp = 0;
  unsigned char c;
  int s = 0;
  h.toUpperCase();
  for (i = h.length() - 1; i >= 0 ; i--)
  {
    // take the char starting from the right
    c = h[i];
    // convert from hex to int
    c = c - '0';
    if (c > 9)
      c = c - 7;
    // add and shift of 4 bits per each char
    tmp += c << s;
    s += 4;
  }
  return tmp;
}


//+=============================================================================
// Send IR codes to variety of sources
//

void handleIr(){
  String type;
  unsigned long code; 
  int len;
  int address;
  type.toLowerCase();
  //long data = HexToLongInt(code);
  
  for (uint8_t i=0; i<server.args(); i++){
   if(server.argName(i) == "code"){
      unsigned long code = strtoul(server.arg(i).c_str(), NULL, 10);
     // long code = HexToLongInt(code);
   }
   if(server.argName(i) == "type"){
      String type = server.arg(i);
   }
   if(server.argName(i) == "len"){
      int len = atoi(server.arg(i).c_str());
   }
   if(server.argName(i) == "address"){
   int address = atoi(server.arg(i).c_str());
   }
          if(type == "nec") {
                irsend.sendNEC(code, len, 0);
          }else if(type == "sony") {
                irsend.sendSony(code, len);
          }else if(type ==  "coolix") {
                irsend.sendCOOLIX(code, len);
          }else if(type == "whynter"){
                irsend.sendWhynter(code, len);
          }else if(type == "panasonic") {
                irsend.sendPanasonic(address, code);
          }else if(type == "jvc") {
                irsend.sendJVC(code, len, 0);
          }else if (type == "samsumg") {
                irsend.sendSAMSUNG(code, len);
                 server.send(200, "text/html", "<html><head> <title>Samsumg</title></head>");
          }else if (type == "sharp") {
                irsend.sendSharpRaw(code, len);
          }else if (type == "dish") {
                irsend.sendDISH(code, len);
          }else if (type == "rc5") {
                irsend.sendRC5(code, len);
          }else if (type == "rc6") {
                irsend.sendRC6(code, len);
          } else {  // necessário definir um else default :o)
             // irsend.sendSAMSUNG(code, 32);
          }   
      
      //}
   // }
  }
  handleRoot();
}


void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void){  
  irsend.begin();
  
//  Serial.begin(115200);
  WiFi.begin(ssid, password);
//  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
//  Serial.println("");
//  Serial.print("Connected to ");
//  Serial.println(ssid);
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
//    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/ir", handleIr); 
 
  server.on("/inline", [](){server.send(200, "text/plain", "this works as well");});

  server.onNotFound(handleNotFound);
  
  server.begin();
//  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 
