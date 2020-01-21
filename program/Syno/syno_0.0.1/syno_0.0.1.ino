#include <Wire.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <SPI.h>
#include <Servo.h>
#include <Pixy2I2C.h>

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "Pixy2_fig.h"
#include "nRF24L01_fig.h"
#include "FUNCTION_fig.h"

void setup() {
  
  pixy.init();
  Wire.begin();
  Serial.begin(2000000);
  
  //USING FOR DEBUGGING --DEBUG
  if(Serial.available()){ 
    debug = 1;
  }else{
    debug = 0;
  }

  //INPUT SET UP --INSETUP
  pinMode(RC_1,INPUT);
  pinMode(RC_2,INPUT);
  pinMode(RC_3,INPUT);
  pinMode(RC_4,INPUT);
  pinMode(RC_5,INPUT);

  //RC_RADIO ATTACH --OUTSETUP
  CH1.attach(ch1);
  CH2.attach(ch2);
  CH3.attach(ch3);
  CH4.attach(ch4);
  CH5.attach(ch5);
  CH6.attach(ch6);

  CH1.writeMicroseconds(900);
  CH2.writeMicroseconds(900);
  CH3.writeMicroseconds(900);
  CH4.writeMicroseconds(900);
  CH5.writeMicroseconds(900);
  CH6.writeMicroseconds(900);

  //nRF24L01 SET UP --RFSETUP
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipe[1]);
  radio.setAutoAck(0);
  radio.stopListening();

}

void loop() {

}

//Pixy2 function
void LINE_DETECT() {
  //OUTLINE: pixy_angle , pixy_middle , x0 , x1 , y0 , y1 , LINE_GET 
  Wire.beginTransmission(0x53);
  Wire.write(0xae);
  Wire.write(0xc1);
  Wire.write(0x30);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.write(0x07);
  Wire.endTransmission();
  Wire.requestFrom(0x53, 14);

  if (Wire.available()) {
    uint16_t Head1 = Wire.read();
    uint16_t Head2 = Wire.read();
    uint8_t typr = Wire.read();
    uint8_t payload = Wire.read();
    uint8_t checksum1 = Wire.read() ;
    uint8_t checksum2 = Wire.read();
    if (typr == 49) {
      int i = 0;
      while (Wire.available()) {

        int8_t feature = Wire.read();
        int8_t length = Wire.read();
        x0 = Wire.read();
        y0 = Wire.read();
        x1 = Wire.read();
        y1 = Wire.read();
        int8_t data5 = Wire.read();
        int8_t data6 = Wire.read();

      }
      pixy_angle = atan((float)(y1 - y0) / (x1 - x0)) * 57.296;
      if (pixy_angle < 0) {
        pixy_angle = map(pixy_angle, -90, 0, 0, -90);
      } else if (pixy_angle > 0) {
        pixy_angle = map(pixy_angle, 90, 0, 0, 90);
      }
      pixy_middle = ((x1 + x0)  - 78) / 2;

      LINE_COUNTER = 0;
      if (pixy_middle < -100) {
        pixy_angle = 0;
        pixy_middle = 0;
        x0 = 39;
        x1 = 39;
        y0 = 26;
        y1 = 26;
        LINE_GET = 0;
        if(debug){
          Serial.println(F("NO LINE"));
        }
      } else {
        LINE_GET = 1;
        if(debug){
          Serial.println(F("----LINE DETECT----"));
          Serial.print(F("Line_middle: "));
          Serial.print(pixy_middle);
          Serial.print(F("  Line_angle: "));
          Serial.println(pixy_angle);
        }
      }
    }
  }
}
void COLOR_DETECT() {
  //OUTLINE: r , g , b
  pixy.changeProg("video");
  if (pixy.video.getRGB(pixy.frameWidth / 2, pixy.frameHeight / 2, &r, &g, &b) == 0)
  {
    if(debug){
      Serial.print(F("----COLOR DETECT----"));
      Serial.print(F("red:"));
      Serial.print(r);
      Serial.print(F(" green:"));
      Serial.print(g);
      Serial.print(F(" blue:"));
      Serial.println(b);
    }
  } else {
    r = 0;
    g = 0;
    b = 0;
  }
}
void CCC_DETECT_TAKE_OFF_POINT_OLD() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {
    
    if(debug){
      Serial.println("----CCC_DETECT_TAKE_OFF_POINT_OLD----");
      Serial.print(F("Detected "));
      Serial.println(pixy.ccc.numBlocks);
    }
    
    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      if (pixy.ccc.blocks[i].m_signature == 7) {
        CCC_ATTACH_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -157, 157);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      }
    }
  } else {
    CCC_ATTACH_CHECK = 0;
    pixy_ccc_x = 0;
    pixy_ccc_y = 0;
  }
}
void CCC_DETECT_COLOR_WAITING() {
  pixy.ccc.getBlocks();
  if (pixy.ccc.numBlocks) {

    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      if (pixy.ccc.blocks[i].m_signature == 1) {

        CCC_ATTACH_CHECK = 1;
        COLOR_READ_CHECK = 0;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      } else if (pixy.ccc.blocks[i].m_signature == 2) {

        CCC_ATTACH_CHECK = 1;
        CCC_ATTACH_COLOR = 1;
        COLOR_READ_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      } else if (pixy.ccc.blocks[i].m_signature == 3) {

        CCC_ATTACH_CHECK = 1;
        CCC_ATTACH_COLOR = 2;
        COLOR_READ_CHECK = 1;
        pixy_ccc_x = map(pixy.ccc.blocks[i].m_x , 0, 315, -158, 158);
        pixy_ccc_y = map(pixy.ccc.blocks[i].m_y , 0, 207, -104, 104);
      }

      if(debug){
        Serial.println(F("----CCC_DETECT_COLOR_WAITING----"));
        Serial.print(F("Detected "));
        Serial.println(pixy.ccc.numBlocks);
        Serial.print(F("X: "));
        Serial.print(pixy_ccc_x);
        Serial.print(F("Y: "));
        Serial.println(pixy_ccc_y);
      }
    }
  } else {
    CCC_ATTACH_CHECK = 0;
    pixy_ccc_x = 0;
    pixy_ccc_y = 0;
  }
}
void CCC_DETECT_COLOR_FINDER() {
  COLOR_DETECT();
  if (r > 250 && g < 150 && b < 150) {
    CCC_ATTACH_CHECK = 1;
  } else {
    CCC_ATTACH_CHECK = 0;
  }
  if(debug){
    Serial.println(F("----CCC_DETECT_COLOR_FINDER----"));
    Serial.println(CCC_ATTACH_CHECK);
  }

}

//RC_INPUT READ
void RC_READING(){
  RC_1_PWM = pulseIn(RC_1,1);
  RC_2_PWM = pulseIn(RC_2,1);
  RC_3_PWM = pulseIn(RC_3,1);
  RC_4_PWM = pulseIn(RC_4,1);
  RC_5_PWM = pulseIn(RC_5,1);

  if(debug){
    Serial.println(F("=====RC_INPUT====="));
    Serial.print(F("RC_1_PWM: "));
    Serial.println(RC_1_PWM);
    Serial.print(F("RC_2_PWM: "));
    Serial.println(RC_2_PWM);
    Serial.print(F("RC_3_PWM: "));
    Serial.println(RC_3_PWM);
    Serial.print(F("RC_4_PWM: "));
    Serial.println(RC_4_PWM);
    Serial.print(F("RC_5_PWM: "));
    Serial.println(RC_5_PWM);
  }
}

//DATA SENDING
void DATA_SEND(){ 
  radio.write(&send_data, sizeof(send_data));
  radio.stopListening();
}
