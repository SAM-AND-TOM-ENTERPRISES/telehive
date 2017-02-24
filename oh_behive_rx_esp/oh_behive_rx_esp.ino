#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

#include "Payload.h"
#include "Config.h"

SoftwareSerial ESPserial(D1, D2); // RX, TX
HTTPClient http;

void setup() {
    // Establish comms with the Moteino serial
    ESPserial.begin(115200);
    Serial.begin(115200);

    // Set up the WiFi wotsits and wait to connect to the defined SSID
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected: ");  
    Serial.println(WiFi.localIP());
    Serial.print("Endpoint: ");
    Serial.println(endpoint);
}


void readFill(void *head, uint8_t sz){
    for(int z = 0; z < sz; z++){
        while(!ESPserial.available());
        ((uint8_t*)head)[z] = ESPserial.read();
    }
}
 
void loop() {
    uint8_t stat = NULL;
    if (ESPserial.available()){
        stat = ESPserial.read();
    }

    if(stat==0x01){
        PLD_climate payload;
        readFill(&payload, sizeof(PLD_climate));
        //printDataPacket(&current);
        post(&payload, stat);
    }
}

void post(void *payload, uint8_t pkt_type) {
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    jsonifyPayload(json, payload, pkt_type);
        
    http.begin(endpoint, fingerprint);
    http.addHeader("Content-Type", "application/json");

    char msg[256];
    json.printTo(msg);
    Serial.print(msg);

    int httpCode = http.POST(msg);
    if (httpCode != HTTP_CODE_OK){
        //TODO(samstudio8) Do something smart if not OK
        Serial.println(" NOTSENT");
    }
    else{
        Serial.println(" SENT");
    }
    http.end();
}



