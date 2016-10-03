#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/*
   ******************************************
   *                                        *
   *                                        *
   *---------------> RGB-WIFI <-------------*
   *                                        *
   *              V 1.1 RELEASE             *
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
 
 >>> IMPROVEMENTS:
     -------------
 
 [NEW]  Use the build-in LED to show incoming WiFi commands.

 [FIX]  Add my header to the Arduino sketch.
 
 [FIX]  The ESP8266 won't create anymore another softAP while connecting with the defined AP.
 
 
 >>> IMPORTANT NOTES: 
     ----------------   
 * There are a lot of free pins on the ESP8266 so you can use them for other stuff if you like.
 * Feel free to modify these sketches but don't hijack them ;-)
 * Commercial use? Contact me! Released under GPLv3.

   (c) Dylan Van Assche (2016) developer of the RGB-WIFI Arduino sketch & Sailfish OS app.
*/

#define LED_PIN D4
#define GREEN_PIN D5
#define RED_PIN D6
#define BLUE_PIN D7

const char *ssid = "WiFi-2.4-3aa4"; // Set here the SSID for your network
const char *password = "7Jm2QwKxM67g"; // And the password

WiFiServer server(80);

void setup(void)
{
  // RGB ledstrip
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 0);
  analogWrite(RED_PIN, 0);
  analogWrite(BLUE_PIN, 0);
  digitalWrite(LED_PIN, LOW);

  // Start serial
  Serial.begin(115200);

  // Start WiFi and connect with the WiFi AP
  WiFi.mode(WIFI_STA); // Force STA mode to avoid that ESP8266 goes into AP_STA mode
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("[INFO] Trying to connect with WiFi AP");

  // Wait for connection with the WiFI AP
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, LOW);
  }
  Serial.println("");
  Serial.print("[INFO] Connected to ");
  Serial.println(ssid);
  Serial.print("[INFO] IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("[INFO] MDNS responder started");
  }
  server.begin();
  Serial.println("[INFO] HTTP server started");
}

void loop(void)
{
  // No client connected? return
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  Serial.println("[INFO] Client connected");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  String response;
  client.flush();

  if (request.indexOf("/red/") != -1)
  {
    digitalWrite(LED_PIN, LOW);
    int redValue = request.substring(9, request.length()).toInt();
    Serial.print("[INFO] RED value: ");
    Serial.println(redValue);
    analogWrite(RED_PIN, redValue);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nRED value: ";
    response += String(redValue);
    response += "</html>\n";
  }
  else if (request.indexOf("/green/") != -1)
  {
    digitalWrite(LED_PIN, LOW);
    int greenValue = request.substring(11, request.length()).toInt();
    Serial.print("[INFO] GREEN value: ");
    Serial.println(greenValue);
    analogWrite(GREEN_PIN, greenValue);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGREEN value: ";
    response += String(greenValue);
    response += "</html>\n";
  }
  else if (request.indexOf("/blue/") != -1)
  {
    digitalWrite(LED_PIN, LOW);
    int blueValue = request.substring(10, request.length()).toInt();
    Serial.print("[INFO] BLUE value: ");
    Serial.println(blueValue);
    analogWrite(BLUE_PIN, blueValue);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nBLUE value: ";
    response += String(blueValue);
    response += "</html>\n";
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    Serial.println("[ERROR] Invalid request");
    analogWrite(RED_PIN, 1023); // Show ERROR since request is invalid
    delay(1000);
    analogWrite(RED_PIN, 0);
    client.stop();
    return;
  }

  client.flush();
  client.print(response); // Send response
  delay(1);
  Serial.println("[INFO] Client disconnected");
  digitalWrite(LED_PIN, HIGH);
}
