#include <FastLED.h>

#define NUM_LEDS 6 //number of led's that we will be adddressing
#define MAX_BRIGHT 200 //set a cap on the brightness level of each led
#define DATA_PIN 7 //pin that we're writing data to

CRGB leds[NUM_LEDS]; //array of leds, each individually addressable 
int brightness = 0; //the current brightness value of the led
int fadeAmount = MAX_BRIGHT%7; //value that we are going to increment/decrement the current brightness by  
int colors[3][3] = {{255,100,100},{255,255,75},{100,100,255}};//array of RGB color values that we want to iterate over
int colorIndex; //index value used to iterate over the color array

void setup() { 
      // What type of LEDs are we using and which order are we sending the data in?
      FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
      // Dim the LEDs
      FastLED.setBrightness(MAX_BRIGHT);
}

void loop() { 
  //reset the loop once we've iterated through all the colors
  //set the color an brightness of each led
  
  for(int i = 0; i < NUM_LEDS; i++ )
  {
    leds[i].setRGB(colors[colorIndex%3][0],colors[colorIndex%3][1],colors[colorIndex%3][2]); 
    brightness += fadeAmount;
    leds[i].maximizeBrightness(brightness%MAX_BRIGHT);
    FastLED.show();
    delay(100);
  }

  int tempColorIndex = colorIndex;

   if (brightness >= MAX_BRIGHT) {
      while(brightness > 0) {
        for(int i = 0; i < NUM_LEDS; i++)
          {
            leds[i].setRGB(colors[tempColorIndex%3][0],colors[tempColorIndex%3][1],colors[tempColorIndex%3][2]); 
            brightness -= fadeAmount;
            leds[i].maximizeBrightness(brightness%MAX_BRIGHT);
            FastLED.show();
            delay(100);
          }
          tempColorIndex++;
      }
   }
   
  colorIndex++;

}
