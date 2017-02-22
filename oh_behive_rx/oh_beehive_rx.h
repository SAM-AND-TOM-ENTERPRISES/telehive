#ifndef __OH_BEEHIVE_RX_H
#define __OH_BEEHIVE_RX_H

#define PKT_TMPHMD 0x01

typedef struct {
  float t1;
  float h1;
  float t2;
  float h2;
  float t3;
  float h3;
  float t4;
  float h4;
  float light;
} Payload;

#endif
