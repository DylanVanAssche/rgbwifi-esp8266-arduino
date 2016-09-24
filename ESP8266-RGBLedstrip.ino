#define GREEN_PIN D5
#define RED_PIN D6
#define BLUE_PIN D7



#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "XXX";
const char *password = "XXX";

WiFiServer server(80);

void setup(void)
{
  // RGB ledstrip
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  analogWrite(GREEN_PIN, 0);
  analogWrite(RED_PIN, 0);
  analogWrite(BLUE_PIN, 0);

  // Start serial
  Serial.begin(115200);

  // Start WiFi and connect with the WiFi AP
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("[INFO] Trying to connect with WiFi AP");

  // Wait for connection with the WiFI AP
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
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
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  String response;
  client.flush();
  
  if (request.indexOf("/red/") != -1)
  {
    int redValue = request.substring(9, request.length()).toInt();
    Serial.print("[INFO] RED value: ");
    Serial.println(redValue);
    analogWrite(RED_PIN, redValue);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nRED value: ";
    response += String(redValue);
    response += "</html>\n";
  }
  else if(request.indexOf("/green/") != -1)
  {
    int greenValue = request.substring(11, request.length()).toInt();
    Serial.print("[INFO] GREEN value: ");
    Serial.println(greenValue);
    analogWrite(GREEN_PIN, greenValue);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGREEN value: ";
    response += String(greenValue);
    response += "</html>\n";
  }
  else if(request.indexOf("/blue/") != -1)
  {
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
}
