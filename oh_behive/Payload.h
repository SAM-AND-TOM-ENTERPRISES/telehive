#define PKT_TMPHMD 0x01

#include <ArduinoJson.h>

typedef struct {
    uint8_t pid;
    float t1;
    float h1;
    float t2;
    float h2;
    float t3;
    float h3;
    float t4;
    float h4;
    float light;
} PLD_climate;

static void printDataPacket(void * pld) {
    uint8_t pkt_type = *((uint8_t *)pld);
    switch(pkt_type){
        case PKT_TMPHMD:
            PLD_climate* p = (PLD_climate*)pld;
            Serial.print(p->pid);
            Serial.print(",");
            Serial.print(p->light);
            Serial.print(",");
            Serial.print(p->h1);
            Serial.print(",");
            Serial.print(p->t1);
            Serial.print(",");
            Serial.print(p->h2);
            Serial.print(",");
            Serial.print(p->t2);
            Serial.print(",");
            Serial.print(p->h3);
            Serial.print(",");
            Serial.print(p->t3);
            Serial.print(",");
            Serial.print(p->h4);
            Serial.print(",");
            Serial.println(p->t4);
            break;
    }
}

static void jsonifyPayload(JsonObject& json, void *pld, uint8_t pkt_type) {
    switch(pkt_type){
        case PKT_TMPHMD:
            PLD_climate* p = (PLD_climate*)pld;
            json["light"] = p->light;
            json["t1"] = p->t1;
            json["h1"] = p->h1;
            json["t2"] = p->t2;
            json["h2"] = p->h2;
            json["t3"] = p->t3;
            json["h3"] = p->h3;
            json["t4"] = p->t4;
            json["h4"] = p->h4;
            json["hoot"] = "hoot";
            break;
    }
}
