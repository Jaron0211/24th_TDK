/*
 * THIS DEFINATION IS FOR PIXY2
 */

Pixy2I2C pixy;

int pixy_middle;
float pixy_angle;
int pixy_ccc_x , pixy_ccc_y;

int8_t x0, y0, x1, y1;
uint8_t r, g, b;

boolean PIXY_CHECK = 0; //CHECK IF IT AVAILABLE
boolean CCC_ATTACH_CHECK = 0;//COLOR BLOCK DETECT BOOLEAN
boolean COLOR_READ_CHECK = 0;//COLOR(RGB) READ CHECK
boolean THROW_CHECK = 0;
unsigned long CCC_ATTACH_TIMER;
unsigned long COLOR_READ_TIMER;
int CCC_ATTACH_COLOR = 0;
int LINE_COUNTER = 0;

boolean TAKE_OFF_CHECK = 0;
boolean PART1_CHECK = 0;
boolean PART2_CHECK = 0;
boolean PART3_CHECK = 0;
boolean PART4_CHECK = 0;
boolean THROW_STARTER = 0;
boolean throw_timer_enable = 0;
boolean stand_check = 0;
boolean MISSION_OVER = 0;
boolean LINE_GET = 0;
boolean BREAK_CHECKER = 0;
