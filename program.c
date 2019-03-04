#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* COMMAND_AC_ON = "TurnOnAC";
const char* COMMAND_AC_OFF = "TurnOffAC";
const char* COMMAND_GATE_OPEN = "OpenGate";
const char* COMMAND_GATE_CLOSE = "CloseGate";
const char* COMMAND_LIGHT_ON = "TurnOnLight";
const char* COMMAND_LIGHT_OFF = "TurnOffLight";

void setup() { 
  Serial.begin(115200);
  WiFi.begin("MAXXITECH", "maxxitech2019");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.println("Waiting for connection");
    pinMode(16, OUTPUT);
    digitalWrite(16,LOW);
    pinMode(5, OUTPUT);
    digitalWrite(5,LOW);
    pinMode(4, OUTPUT);
    digitalWrite(4,LOW);
  }
}
 
void loop() { 
   if(WiFi.status() != WL_CONNECTED){
      Serial.println("Error in WiFi connection");
      return;
   } 
   HTTPClient http;
   http.begin("http://192.168.0.32:84/api/actions");
   http.addHeader("Content-Type", "text/plain");
   int httpCode = http.GET();
   String payload = http.getString();
   if(payload.length() < 20){
      Serial.println("Nothing to do...");
      Serial.println(payload);
      http.end(); 
      delay(1000);
      return;
   }
   
   const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 30;
   DynamicJsonDocument doc(capacity);
   deserializeJson(doc, payload);
   const char* action = doc["actions"]["action"];
   int id = doc["actions"]["id"];
   
   if(strcmp(action, COMMAND_AC_ON) == 0){
        turnOnAC(id);
   }
   else if(strcmp(action, COMMAND_AC_OFF) == 0){
        turnOffAC(id);
   }     
   else if(strcmp(action, COMMAND_GATE_OPEN) == 0){
        openGate(id);
   }
   else if(strcmp(action, COMMAND_GATE_CLOSE) == 0){
        closeGate(id);
   }
   else if(strcmp(action, COMMAND_LIGHT_ON) == 0){
        turnOnLight(id);
   }
   else if(strcmp(action, COMMAND_LIGHT_OFF) == 0){
        turnOffLight(id);
   }   
   else{
        invalidCommand(action);
   }
   
  http.end(); 
  delay(500);  
}

void turnOnAC(int id){
    Serial.println("turning the AC on...");
  digitalWrite(16,HIGH);
  confirmAction(id);
}
void turnOffAC(int id){
    Serial.println("turning the AC off...");
  digitalWrite(16,LOW);
  confirmAction(id);
}

void turnOnLight(int id){
    Serial.println("turning the light on...");
  digitalWrite(4,HIGH);
  confirmAction(id);  
}

void turnOffLight(int id){
      Serial.println("turning the light off...");
  digitalWrite(4,LOW);
  confirmAction(id);
}

void openGate(int id){
  Serial.println("opening gate...");
  digitalWrite(5,HIGH);
  confirmAction(id);
}

void closeGate(int id){
  Serial.println("closing gate...");
  digitalWrite(5,LOW);
  confirmAction(id);
}

void confirmAction(int id){
   HTTPClient http;
   http.begin("http://192.168.0.32:84/api/actions/" + String(id));
   http.addHeader("Content-Type", "application/json");
   int httpCode = http.PUT(String(id));
  Serial.println("confirming " + String(httpCode));
   http.end();
}

void invalidCommand(const char* action){
  Serial.print(action);
  Serial.println(" - invalid command");
}
