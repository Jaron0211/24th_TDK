/*
 * --WAITADD : NEED TO ADJUST FUNCTION DETIAL
 * --DEBUG : DEBUG OPTION
 * --INSETUP : INPUT SETUP TAG
 * --OUTSETUP : OUTPUT SETUP TAG 
 * --RFSETUP : nRF24L01 SETUP TAG
 * --AMCHSE : AUTO/MANUAL MODE CHOOSE TAG
 * --TAKEOFF : TAKE OFF FUNCTION TAG
 * --ISRSETUP : ISR SETUP TAG
 * --ISRPRO : ISR PROGRAM TAG
 */

#include <Wire.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include <SPI.h>

#include "global.h"
#include "INPUT_PIN.h"
#include "OUT_PIN.h"
#include "nRF24L01_fig.h"
#include "FUNCTION_fig.h"
#include "PWM.hpp"

PWM CH1(2);
PWM CH2(3);
PWM CH3(18);
PWM CH4(19);
PWM CH5(20);

void setup() {
  
  Wire.begin();
  //USING FOR DEBUGGING --DEBUG
  //debug = 1;
  if(debug){
    Serial.begin(19200);
  }
  
  //INPUT SET UP --INSETUP
  CH1.begin(true);
  CH2.begin(true);
  CH3.begin(true);
  CH4.begin(true);
  CH5.begin(true);   
  
  //RC_RADIO ATTACH --OUTSETUP
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

  ppm[2] = 1000;

  //nRF24L01 SET UP --RFSETUP
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(1);
  radio.setDataRate(RF24_2MBPS);//MAX LENGTH IS 0.25MB/S
  radio.setCRCLength(RF24_CRC_8);
  radio.openWritingPipe(pipe[1]);
  radio.setAutoAck(0);
  radio.stopListening();

  //ISR SETUP --ISRSETUP
  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

}

void loop() {

  if(Serial.available()){
    delay(10);
    while(Serial.available()){ 
      Serial.print(char(Serial.read()));
    }
    Serial.println();
  }
  
  DATA_SEND();
  RC_READING();

  //FUNCTION CHOOSE --AMCHSE
  if(MODE_1_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_1_PWM + 50 ){
    MANUAL();
  }else if (MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50){
    AUTO();
  }
  
}



//MANUAL FUNCTION
void MANUAL(){

  RC_READING();
  
  ppm[0] = RC_1_PWM;
  ppm[1] = RC_2_PWM;
  ppm[2] = RC_3_PWM;
  ppm[3] = RC_4_PWM;
  ppm[4] = 970;
  
}


//AUTO FUNCTION
void AUTO(){
  
  if(debug){
    Serial.println(F("=====AUTO_LOG====="));
  }
  
  //TAKE OFF --TAKEOFF
  TAKE_OFF_CHECK = 0;
  while(1){
    //FOR BREAKING
    RC_READING();
    if((MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50) and TAKE_OFF_CHECK == 0){
      TAKE_OFF();
    }else{
      break;
    }
  }
 
}


//DRONE BASIC FUNCTION
void TAKE_OFF(){
  //ARM
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    //FOR BREAKING
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }    
    ppm[0] = 1400 ;
    ppm[1] = 1400 ;
    ppm[2] = 1000 ;
    ppm[3] = 1790 ;
    ppm[4] = 1400 ;
    
    if(millis() - timer > 6000){
      break; 
    }
  }
  //READY TO TAKE OFF
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }

    ppm[0] = 1400;
    ppm[1] = 1400;
    ppm[2] = 1000;
    ppm[3] = 1400;
    ppm[4] = 1400;
    
    if (millis() - timer > 3000) {
      break;
    }
  }
  ///TAKE_OFF --WAITADD
  if(TAKE_OFF_CHECK == 0){
    timer = millis();
  }
  while(1){
    //FOR BREAKING
    RC_READING();
    if(!(MODE_2_PWM - 50 < RC_5_PWM and RC_5_PWM < MODE_2_PWM + 50)){
      break;
    }    
    ppm[0] = 1400 ;
    ppm[1] = 1400 ;
    ppm[2] = 1510 ;
    ppm[3] = 1400 ; //NEED ADD TAKE OFF STABLIZE (USE OPENMV H7)
    ppm[4] = 1400 ;
    
    if(millis() - timer > 2500){
      TAKE_OFF_CHECK = 1;
      break; 
    }
  }
}
void THROW(){

}
void RC_READING(){
  
  RC_1_PWM = CH1.getValue();
  RC_2_PWM = CH2.getValue();
  RC_3_PWM = CH3.getValue();
  RC_4_PWM = CH4.getValue();
  RC_5_PWM = CH5.getValue();


  if(0/*debug*/){
    //Serial.println(F("=====RC_INPUT====="));
    Serial.print(RC_1_PWM);
    Serial.print(F(" "));
    Serial.print(RC_2_PWM);
    Serial.print(F(" "));
    Serial.print(RC_3_PWM);
    Serial.print(F(" "));
    Serial.print(RC_4_PWM);
    Serial.print(F(" "));
    Serial.println(RC_5_PWM);
  }
}
void DATA_SEND(){ 
  radio.write(&send_data, sizeof(send_data));
  radio.stopListening();
}


// ISR PROGRAM --ISRPRO
ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if(state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= chanel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;// 
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
