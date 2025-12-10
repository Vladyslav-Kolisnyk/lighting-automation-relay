#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <stdint.h>

#include "config.h"

unsigned long lastTime = 0;
uint8_t relayStateValue;

void connectToWiFi();
String requestLightingSensor();
uint8_t getRelayState();

void setup() 
{
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(BAUD);
  connectToWiFi();
}

void loop() 
{
  if ((millis() - lastTime) > DELAY_MS) 
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      relayStateValue = getRelayState();

      if (relayStateValue == 1 || relayStateValue == 0)
      {
        digitalWrite(RELAY_PIN, relayStateValue);
      }
      else
      {
        Serial.println("Received relay state is invalid!"); 
      }
    }
    else 
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void connectToWiFi()
{
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

String requestLightingSensor() 
{
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, LIGHTING_SENSOR_URL);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}

uint8_t getRelayState()
{
  String relayState = requestLightingSensor();
  Serial.println(relayState);
  JSONVar parsedObject = JSON.parse(relayState);
 
  if (JSON.typeof(parsedObject) == "undefined") 
  {
    Serial.println("Parsing input failed!");
    return 2;
  }
    
  Serial.print("JSON object = ");
  Serial.println(parsedObject);
    
  JSONVar keys = parsedObject.keys();
  JSONVar value = parsedObject[keys[0]];

  return uint8_t(value);
}
