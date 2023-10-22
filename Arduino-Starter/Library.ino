#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define MOTOR1_ENABLE 27
#define MOTOR1_PHASE 26
#define MOTOR2_ENABLE 21
#define MOTOR2_PHASE 20

uint8_t TARGET_ADDRESS[] = {0xF0,0xF0,0xF0,0xF0,0xD2};
uint8_t LOCAL_ADDRESS[] = {0xF0,0xF0,0xF0,0xF0,0xE1};
#define CHANNEL 76 // 0-125
#define RF_POWER_LEVEL RF24_PA_MAX

RF24 radio(22,1);

uint8_t radio_packet[7];

bool buttonA() {
  return (radio_packet[5] >> 0) & 1;
}
bool buttonB() {
  return (radio_packet[5] >> 1) & 1;
}
bool buttonX() {
  return (radio_packet[5] >> 2) & 1;
}
bool buttonY() {
  return (radio_packet[5] >> 3) & 1;
}
bool buttonLB() {
  return (radio_packet[5] >> 4) & 1;
}
bool buttonRB() {
  return (radio_packet[5] >> 5) & 1;
}
bool buttonBack() {
  return (radio_packet[5] >> 6) & 1;
}
bool buttonStart() {
  return (radio_packet[5] >> 7) & 1;
}
bool buttonL3() {
  return (radio_packet[6] >> 0) & 1;
}
bool buttonR3() {
  return (radio_packet[6] >> 1) & 1;
}
float axisLX() {
  float lx = (((float)(radio_packet[0])) / 127.5) - 1.0;
  lx = constrain(lx, -1.0, 1.0);
  if(lx >= -0.05 && lx <= 0.05) return 0.0;
  return lx;
}
float axisLY() {
  float ly = (((float)(radio_packet[1])) / 127.5) - 1.0;
  ly = constrain(ly, -1.0, 1.0);
  if(ly >= -0.05 && ly <= 0.05) return 0.0;
  return ly;
}
float axisRX() {
  float rx = (((float)(radio_packet[2])) / 127.5) - 1.0;
  rx = constrain(rx, -1.0, 1.0);
  if(rx >= -0.05 && rx <= 0.05) return 0.0;
  return rx;
}
float axisRY() {
  float ry = (((float)(radio_packet[3])) / 127.5) - 1.0;
  ry = constrain(ry, -1.0, 1.0);
  if(ry >= -0.05 && ry <= 0.05) return 0.0;
  return ry;
}
float axisLT() {
  float lt = (((float)(radio_packet[4])) - 127.5) / 127.5;
  lt = constrain(lt, 0.0, 1.0);
  if(lt <= 0.05) return 0.0;
  return lt;
}
float axisRT() {
  float rt = (127.5 - ((float)(radio_packet[4]))) / 127.5;
  rt = constrain(rt, 0.0, 1.0);
  if(rt <= 0.05) return 0.0;
  return rt;
}
int dpad() {
  return radio_packet[6] / 4;
}

void setup1() {
  pinMode(MOTOR1_ENABLE, OUTPUT);
  pinMode(MOTOR1_PHASE, OUTPUT);
  pinMode(MOTOR2_ENABLE, OUTPUT);
  pinMode(MOTOR2_PHASE, OUTPUT);
  
  SPI.setTX(3);
  SPI.setRX(0);
  SPI.setSCK(2);
  SPI.setCS(1);
  SPI.begin();
  
  radio.begin();
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(CHANNEL);
  radio.setCRCLength(RF24_CRC_16);
  radio.setPALevel(RF_POWER_LEVEL);
  radio.openWritingPipe(TARGET_ADDRESS);
  radio.openReadingPipe(1,LOCAL_ADDRESS);
  radio.startListening();
}

bool isPacketValid(uint8_t *pkt) {
  if(pkt[0] != pkt[11]) return false;
  if(pkt[1] != pkt[12]) return false;
  if(pkt[2] != pkt[13]) return false;
  if(pkt[3] != pkt[14]) return false;
  
  uint8_t sum = 0;
  for(int i = 0; i < 15; i++) {
    sum += pkt[i];
  }
  if(sum != pkt[15]) return false;
  
  return true;
}

void loop1() {
  if(radio.available()) {
    uint8_t buf[20];
    radio.read(buf, sizeof(buf));

    if(isPacketValid(buf+1)) {
      memcpy(radio_packet, buf+5, 7);
    } else if(isPacketValid(buf+2)) {
      memcpy(radio_packet, buf+6, 7);
    } else {
      // Invalid
      Serial.print("Invalid ");
      for(int i = 0; i < 20; i++) {
        Serial.print(buf[i]);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
}

void drive(float l, float r) {
  l = constrain(l, -1.0, 1.0);
  r = constrain(r, -1.0, 1.0);
  
  if(l > 0.0) {
    analogWrite(MOTOR2_ENABLE, int(255 * l));
    digitalWrite(MOTOR2_PHASE, HIGH);
  } else {
    analogWrite(MOTOR2_ENABLE, int(-255 * l));
    digitalWrite(MOTOR2_PHASE, LOW);
  }
  
  if(r > 0.0) {
    analogWrite(MOTOR1_ENABLE, int(255 * r));
    digitalWrite(MOTOR1_PHASE, HIGH);
  } else {
    analogWrite(MOTOR1_ENABLE, int(-255 * r));
    digitalWrite(MOTOR1_PHASE, LOW);
  }
}
