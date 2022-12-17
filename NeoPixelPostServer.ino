#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>  // NEOPIXEL Library

/*
* Neopixel stuff
*/
#define PIN0 D7
#define PIN1 D8

Adafruit_NeoPixel pix0 = Adafruit_NeoPixel(80, PIN0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pix1 = Adafruit_NeoPixel(80, PIN1, NEO_GRB + NEO_KHZ800);

#ifndef STASSID
#define STASSID "YOUR_SSID"
#define STAPSK  "YOUR_PSK"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const String postForms = "<html>\
  <head>\
    <title>Thekenbeleuchtung - POST-Server</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>POST JSON text to /post/</h1>\
    <p>\"R\": (int)</p>\
    <p>\"G\": (int)</p>\
    <p>\"B\": (int)</p>\
  </body>\
</html>";

void handleRoot() {
  server.send(200, "text/html", postForms);
}

void handlePost() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
      Serial.println(server.argName(i) + "........" + server.arg(i));
    }
    
    if (true) {
      //JSONSTUFF
      StaticJsonDocument<200> doc;
      String json = server.arg(0);
      DeserializationError error = deserializeJson(doc, json);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      int r = doc["r"];
      int g = doc["g"];
      int b = doc["b"];
      setColor(80, r, g, b, 0);
    }
    server.send(200, "text/plain", message);
  }
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

void setup(void) {
  pix0.begin();
  pix1.begin();
  setColor(80, 255, 255, 255, 10);
  Serial.begin(115200);
  String newHostname = "ESP8266_THEKE";
  WiFi.hostname(newHostname.c_str());
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/post", handlePost);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

void setColor(int led, int redValue, int greenValue, int blueValue, int delayValue) {
  int k;
  for (k=0; k <= led; k++) {
    pix0.setPixelColor(k, pix0.Color(redValue, greenValue, blueValue));
    pix1.setPixelColor(k, pix1.Color(redValue, greenValue, blueValue));
  }
  pix0.show();
  pix1.show();
  delay(delayValue);
}