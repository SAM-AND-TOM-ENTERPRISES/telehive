#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#include <ArduinoJson.h>

#include "Payload.h"

#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30
#define DHT1PIN 3
#define DHT2PIN 4
#define DHT3PIN 5
#define DHT4PIN 6

#define RGBPIN 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, RGBPIN, NEO_GRB + NEO_KHZ800);


byte sendSize = 0;
boolean requestACK = false;
SPIFlash flash(8, 0xEF30); //EF40 for 16mbit windbond chip
RFM69 radio;

PLD_climate theData;
#define DHTTYPE     DHT22
#define LIGHTPIN A7
DHT  dht1(DHT1PIN, DHTTYPE);
DHT  dht2(DHT2PIN, DHTTYPE);
DHT  dht3(DHT3PIN, DHTTYPE);
DHT  dht4(DHT4PIN, DHTTYPE);

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LIGHTPIN, INPUT);
  pinMode(DHT1PIN, INPUT_PULLUP);
  pinMode(DHT2PIN, INPUT_PULLUP);
  pinMode(DHT3PIN, INPUT_PULLUP);
  pinMode(DHT4PIN, INPUT_PULLUP);
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower();
  radio.encrypt(KEY);

  theData.pid = PKT_TMPHMD;
  
  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  if (flash.initialize())
    Serial.println("SPI Flash Init OK!");
  else
    Serial.println("SPI Flash Init FAIL! (is chip present?)");

  Serial.println("Init the DHT sensors");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();

}

void loop() {

  readDHT();
  readLight();
  printDataPacket(&theData);
  Serial.print("Sending struct (");
  Serial.print(sizeof(theData));
  Serial.print(" bytes) ... ");
  
  if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
  {
    Serial.print(" ok!");
  }
  else Serial.print(" nothing...");
  {
    Serial.println();
  }
  //Blink(LED, 3);
 
  int leds = theData.light / 85;
  
  
  uint32_t rgb = strip.Color(255,0,0);
  for (int i = 0; i < strip.numPixels(); i++) {
    if(i < leds)
      {strip.setPixelColor(i, rgb);}
      else
      {
        strip.setPixelColor(i, 0);
      }
      
  }
  strip.show();
  delay(1000);
}

static inline void readLight()
{
  theData.light = analogRead(LIGHTPIN);
}

static inline void readDHT()
{
  theData.h1 = dht1.readHumidity();
  theData.t1 = dht1.readTemperature();
  theData.h2 = dht2.readHumidity();
  theData.t2 = dht2.readTemperature();
  theData.h3 = dht3.readHumidity();
  theData.t3 = dht3.readTemperature();
  theData.h4 = dht4.readHumidity();
  theData.t4 = dht4.readTemperature();
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}

