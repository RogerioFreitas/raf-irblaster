#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <IRremoteESP8266.h>
#include <Ticker.h>

<<<<<<< HEAD
//#ifdef ARDUINO_ESP8266_ESP01
//  #define DEBUG false
//#else
//  #define DEBUG true
//#endif
=======
#ifdef ARDUINO_ESP8266_ESP01
  #define DEBUG false
#else
  #define DEBUG true
#endif
>>>>>>> origin/develop

bool debug = false; // :(

Ticker ticker;

ESP8266WebServer server(80);
WiFiManager wifiManager;

IRsend irsend(3);

void setup(void) {
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.1, tick);
  
  if (debug) {
    Serial.begin(9600);
    Serial.println("Hello :D");
  }

  wifiManager.setDebugOutput(debug);
  wifiManager.setConfigPortalTimeout(120);
<<<<<<< HEAD
  //wifiManager.autoConnect("IrBlaster");

  //for some reason that I can't understood why at my home, I only have sucess to connect to wifi AP using the code below :(

  if (!wifiManager.autoConnect("IrBlaster")) {
    //Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
=======
  wifiManager.autoConnect("IrBlaster");
>>>>>>> origin/develop

  ticker.attach(0.5, tick);  

  irsend.begin();

  if (debug) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  ticker.attach(1, tick);  

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/reset", []() {
    server.send(200, "text/plain", "Rebooting in config mode");
    wifiManager.resetSettings();
    ESP.reset();
  });

  server.onNotFound(handleNotFound);

  server.begin();

  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);
}


void handleRoot() {
  server.send(200, "text/html", "<html><head> <title>IR Blaster</title></head><body><center><h1>IR BLASTER!</h1><br/><img src=\"https://i.giphy.com/3oEduGi1UWg9Q6nF84.webp\" width=\"100%\"</center></body></html>");
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
void handleIr() {
  if (server.args() == 0) {
    server.send(200, "application/json", "{}");
    return;
  }

  String type = "";
  unsigned long code = 0;
  int len = 0;
  int address = 0;
  type.toLowerCase();

  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i)  == "code") {
      String strCode = server.arg(i);
      strCode.toLowerCase();
      if (strCode.startsWith("0x")) {
        code = HexToLongInt(strCode.substring(2));
      } else {
        code = strtoul(strCode.c_str(), NULL, 10);
      }
    } else if (server.argName(i) == "type") {
      type = server.arg(i);
    } else if (server.argName(i) == "len") {
      len = atoi(server.arg(i).c_str());
    } else if (server.argName(i) == "address") {
      address = atoi(server.arg(i).c_str());
    }
  }

  if (debug) {
    Serial.print("Type: ");
    Serial.println(type);
    Serial.print("Code: ");
    Serial.println(code);
    Serial.print("Len: ");
    Serial.println(len);
    Serial.print("Address: ");
    Serial.println(address);
  }

  for (int r = 0; r <= 3; r++) {
    if (type == "nec") {
      if (len == 0)
        len = 32;
      irsend.sendNEC(code, len, 0);
    } else if (type == "sony") {
      if (len == 0)
        len = 12;
      irsend.sendSony(code, len);
    } else if (type ==  "coolix") {
      irsend.sendCOOLIX(code, len);
    } else if (type == "whynter") {
      irsend.sendWhynter(code, len);
    } else if (type == "panasonic") {
      irsend.sendPanasonic(address, code);
    } else if (type == "jvc") {
      irsend.sendJVC(code, len, 0);
<<<<<<< HEAD
    } else if (type == "samsung") {
      if (len ==0)
        len = 32;
=======
    } else if (type == "samsumg") {
>>>>>>> origin/develop
      irsend.sendSAMSUNG(code, len);
    } else if (type == "sharp") {
      irsend.sendSharpRaw(code, len);
    } else if (type == "dish") {
      irsend.sendDISH(code, len);
    } else if (type == "rc5") {
      irsend.sendRC5(code, len);
    } else if (type == "rc6") {
      irsend.sendRC6(code, len);
    } else {
      server.send(400, "application/json", "{\"error\": \"unknown type " + type + "\"}");
      return;
    }
<<<<<<< HEAD
  }

  String json = "{\"status\": \"OK\"";
  for (uint8_t i = 0; i < server.args(); i++) {
    json += ", \"" + server.argName(i) + "\": \"" + server.arg(i) + "\"";
  }
=======
  }

  String json = "{\"status\": \"OK\"";
  for (uint8_t i = 0; i < server.args(); i++) {
    json += ", \"" + server.argName(i) + "\": \"" + server.arg(i) + "\"";
  }
>>>>>>> origin/develop
  json += "}";
  server.send(404, "text/plain", json);

}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void tick() {
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void loop(void) {
  server.handleClient();
}
<<<<<<< HEAD
=======

>>>>>>> origin/develop
