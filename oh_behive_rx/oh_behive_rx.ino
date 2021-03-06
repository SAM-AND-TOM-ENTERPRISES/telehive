#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <ArduinoJson.h>
#include "Payload.h"


#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

RFM69 radio;
SPIFlash flash(8, 0xEF30); //EF40 for 16mbit windbond chip
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network



void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  radio.promiscuous(promiscuousMode);

  //TODO: This stuff probably wants reporting to some form of debug?
  // char buff[50];
  // sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  // Serial.println(buff);


  if (flash.initialize())
  {
    //Serial.println("SPI Flash Init OK!");
  }
  else
  {
    //Serial.println("SPI Flash Init FAIL! (is chip present?)");
  }
}


void loop() {
  if (radio.receiveDone())
  {
    switch (radio.DATA[0])
    {
      case PKT_TMPHMD:
        {
          if (radio.DATALEN == sizeof(PLD_climate))
          {
            PLD_climate theData = *(PLD_climate*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
            writePacketToSerial(&theData, sizeof(PLD_climate));
          }
          else
          {
            //TODO: REPORT ERROR
            // Serial.print("Invalid payload received, not matching Payload struct!");
          }

          break;
        }
    }

    //This should probably be sent sooner, excessive delay could result in retransmissions
    if (radio.ACKRequested())
    {
      radio.sendACK();
    }

    //Blink(LED,3);
  }
}

//TODO: Candidate for generalising
/**
 * Writes a Data Packet to the serial in byte form.
 */
static void writePacketToSerial(void * pld, size_t sz)
{
  //Get pointer to struct & size
  uint8_t * dp = (uint8_t *)pld;

  //Write Struct
  for (int i = 0; i < sz; i++)
  {
    Serial.write((uint8_t)*dp);
    dp++;
  }
}

/**
 * Generic shitty, blocking led blinky function
 */
void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN, LOW);
}
