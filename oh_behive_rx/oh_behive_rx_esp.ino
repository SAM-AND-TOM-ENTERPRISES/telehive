#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid     = "ssid";
const char* password = "pass";


SoftwareSerial ESPserial(D1, D2); // RX, TX
HTTPClient http;

char buff[128];
int i = 0;

void setup() {
    // Establish comms with the Moteino serial
    ESPserial.begin(115200);

    Serial.begin(115200);

    // Set up the WiFi wotsits and wait to connect to the defined SSID
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected: ");  
    Serial.println(WiFi.localIP());
}
 
void loop() {
    int c = NULL;
    if (ESPserial.available()){
        // Data available, do something with it...
        c = ESPserial.read();
    }

    if (c){
        if(c == '\n'){
            Serial.println(buff);
            //post(buff); TODO(samstudio8)
            i = 0;            
        }
        else{
            buff[i++] = c;
        }
    }
}

void post(char* payload) {
    Serial.println(payload);
    
    http.begin("http://127.0.0.1:5000/");    
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(buff);
    if (httpCode != HTTP_CODE_OK){
        //TODO(samstudio8) Do something smart if not OK
    }
    else{
        Serial.println("SENT");
    }
    http.end();
}

