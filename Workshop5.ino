// UNSW ILLUMINATE DIGI-DOG WORKSHOP
 
// if the libraries below are not presently available on your machine:
// hit ctrl+shift+i, then search for the library, eg 'ledcontrol'
// install the first one listed for each search, they will be named LedControl and NewPing.

#include <LedControl.h> 
#include <NewPing.h> 
​
#define DISPLAY_ROWS 8
#define DISPLAY_COLS 8
#define NUM_DISPLAYS 2      // number of displays - we need 2, one for each eye
#define MAX_DISTANCE 10000  // the max distance for the ultrasonic pulse
​
#define ECHO_PIN_L    2 // This recieves the ultrasonic chirp
#define TRIG_PIN_L    3 // This sends an ultrasonic chirp
​
#define ECHO_PIN_R    4
#define TRIG_PIN_R    5
​
// These are defined by the SPI (serial programming interface) protocol
#define CLK          9  // clock, needed for synchronising data transmission
#define CS           10 // chip select
#define DIN          11 // data in
​
// define the devices
NewPing eyeL(TRIG_PIN_L, ECHO_PIN_L, MAX_DISTANCE);
NewPing eyeR(TRIG_PIN_R, ECHO_PIN_R, MAX_DISTANCE);
LedControl displays=LedControl(DIN,CLK,CS,NUM_DISPLAYS);
​
byte eye_forward[DISPLAY_COLS] =
{
  0b00111100,
  0b01000010,
  0b01011010,
  0b10101101,
  0b10111101,
  0b10011001,
  0b01000010,
  0b00111100
};
byte eye_right[DISPLAY_COLS] =
{
  0b00111100,
  0b01000010,
  0b01110010,
  0b10011001,
  0b10111001,
  0b10110001,
  0b01000010,
  0b00111100
};
​
byte eye_left[DISPLAY_COLS] =
{
  0b00111100,
  0b01000010,
  0b01001110,
  0b10010111,
  0b10011111,
  0b10001101,
  0b01000010,
  0b00111100
};
​
byte eye_blink[DISPLAY_COLS] =
{
  0b00000000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b10111101,
  0b11000011,
  0b01111110,
  0b00111100
};
​
void setup(){
  // Configure our pins as inputs or outputs
  pinMode(TRIG_PIN_L, OUTPUT); 
  pinMode(ECHO_PIN_L, INPUT);
  pinMode(TRIG_PIN_R, OUTPUT);
  pinMode(ECHO_PIN_R, INPUT);

  Serial.begin(9600);
​
  for(int i=0;i<NUM_DISPLAYS;i++) { // For each display:
    displays.shutdown(i,false);       // Disable the MAX72XX power-saving mode
    displays.setIntensity(i,10);      // Set the brightness to maximum value
    displays.clearDisplay(i);         // and clear the display
  }
}
​
void loop(){ 
  digitalWrite(TRIG_PIN_L, HIGH);
  digitalWrite(TRIG_PIN_R, HIGH);
  
  long duration1 = pulseIn(ECHO_PIN_L, HIGH);
  int distance1 = duration1 * 0.034 / 2;
  distance1 = eyeL.ping_median( 5 );
  
  long duration2 = pulseIn(ECHO_PIN_R, HIGH);
  int distance2 = duration2 * 0.034 / 2;
  distance2 = eyeR.ping_median( 5 );
​
  if (abs(distance1 - distance2) < 500) {
    printByte(eye_forward);
  } else if (distance1 > distance2) {
    printByte(eye_left);
  } else if (distance1 < distance2) {
    printByte(eye_right);
  }
}
​
void printByte(byte character [])
{
  for(int i=0;i<8;i++) {
    displays.setRow(0,i,character[i]);
    displays.setRow(1,i,character[i]);
  }
}
