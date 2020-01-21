/*
 * THIS IS FOR THE RF24
 */

RF24 radio(47, 46);
const uint64_t pipe[2] = { 0xE8E8F0F0A1LL, 0xE8E8F0F0A2LL } ;
unsigned int RECEIVER_CHECK = 100;

typedef struct {

  int CH1;
  int CH2;
  int CH3;
  int CH4;
  int CH5;
  int CH6;
  int MODE;
  int __SAVE__;

} SEND_DATA;

SEND_DATA send_data;
