#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

/*
   ******************************************
   *                                        *
   *                                        *
   *---------------> RGB-WIFI <-------------*
   *                                        *
   *             V 2.0-0 RELEASE            *
   ******************************************
   THIS IS A RELEASE! I'm never resposibly for any damage to your stuff! This version is tested on a NodeMCU V1.0 ESP8266.
   Control your RGB ledstrip over a WiFi connection with your Sailfish OS smartphone and his app.
   The sketch is based on the Arduino examples which are delivered together with the ESP8266 Arduino core.
   

 >>> FEATURES:
     ---------
 
 * Control your RGB LED strip from your Sailfish OS smartphone.
 * Simple HTTP API to control the 3 colours (red - green - blue).
 * Arduino compatible and uses the widly available & cheap ESP8266 WiFi module as CPU & WiFi controller.
 * Uses WPA/WEP security (depends on your router) to connect with your local network.
 * By opening a port in your router you can control it from outside your local network.
 * Responses in JSON encoded format.
 
 >>> IMPROVEMENTS:
     -------------
 
 [NEW]  Response in JSON encoded format for easy parsing

 [FIX]  <Connection Closed> error when using QNetworkAccessManager is now fixed

 >>> ENDPOINTS:
     ----------
 * /light          ->  returns the current color values
 * /light?red=value&green=value&blue=value  ->  updates the color values and returns them
 
 
 >>> IMPORTANT NOTES: 
     ----------------   
 * There are a lot of free pins on the ESP8266 so you can use them for other stuff if you like.
 * Feel free to modify these sketches but don't hijack them ;-)
 * Commercial use? Contact me! Released under GPLv3.

   (c) Dylan Van Assche (2017) developer of the RGB-WIFI Arduino sketch & Sailfish OS app.
*/

#define LED_PIN D4
#define GREEN_PIN D5
#define RED_PIN D6
#define BLUE_PIN D7

const char* ssid     = "XXX";
const char* password = "XXX";

ESP8266WebServer server(80);
StaticJsonBuffer<200> jsonBuffer;

int redValue = 0;
int greenValue = 0;
int blueValue = 0;
 
void setup(void) {
  
  // RGB ledstrip
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);

  // ONBOARD status LED
  digitalWrite(LED_PIN, LOW);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  bool ledstate = LOW;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    ledstate = !ledstate;
    digitalWrite(LED_PIN, ledstate);
  }

  // Server endpoints
  server.onNotFound(replyError);
  server.on("/light", setRGBA);

  // Start server
  server.begin();
  digitalWrite(LED_PIN, HIGH);
}
 
void loop(void) {

  // Handle the client
  server.handleClient();
}

/* Wrong endpoint should return a 404 HTTP error code to the client */
void replyError() {
  // Build JSON reply
  jsonBuffer.clear();
  JsonObject& json = jsonBuffer.createObject();
  json["status"] = "endpoint not found";

  // Reply to client
  String reply;
  json.printTo(reply);
  server.send(404, "application/json", reply);
}

/* Set the color and the brightness of the ledstrip using GET arguments */
void setRGBA() {
  
  // Return the current color values
  if(server.args() == 0) {
    // Build JSON reply
    jsonBuffer.clear();
    JsonObject& json = jsonBuffer.createObject();
    json["status"] = "success";
    json["red"] = redValue;
    json["green"] = greenValue;
    json["blue"] = blueValue;

    // Reply to client
    String reply;
    json.printTo(reply);
    server.send(200, "application/json", reply);
  }
  
  // Update the color values (limit them if necessary) and return them
  else if(server.args() == 3) {
    // Update the color values
    redValue = constrain(server.arg(0).toInt(), 0, 1024);
    greenValue = constrain(server.arg(1).toInt(), 0, 1024);
    blueValue = constrain(server.arg(2).toInt(), 0, 1024);
    analogWrite(RED_PIN, redValue);
    analogWrite(GREEN_PIN, greenValue);
    analogWrite(BLUE_PIN, blueValue);

    // Build JSON reply
    jsonBuffer.clear();
    JsonObject& json = jsonBuffer.createObject();
    json["status"] = "success";
    json["red"] = redValue;
    json["green"] = greenValue;
    json["blue"] = blueValue;

    // Reply to client
    String reply;
    json.printTo(reply);
    server.send(200, "application/json", reply);
  }
  
  // Bad request
  else {
    // Build JSON reply
    jsonBuffer.clear();
    JsonObject& json = jsonBuffer.createObject();
    json["status"] = "wrong arguments";

    // Reply to client
    String reply;
    json.printTo(reply);
    server.send(400, "application/json", reply);
  }
}

