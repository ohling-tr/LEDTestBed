#include "FastLED.h"
#define LED_PIN 6
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 12
#define H_PIN A0
#define S_PIN A1
#define V_PIN A2
#define DELAY_PIN A3

#define PATTERN_SW_PIN 22
#define PATTERN_SW_STEPS 6

#define CHASE_LEN_PIN 34
#define CHASE_LEN_STEPS 6
#define CHASE_LEN_INCRMNT 2

CRGB leds[NUM_LEDS];
int chase_offset = 0;
int tValue = 0;

void setup() {
   // FastLED.addLeds<NEOPIXEL, 6>(leds, 60); 
   FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);

   for (int iPin=22; iPin < 53; iPin++) {
     pinMode(iPin, INPUT_PULLUP);
   }
}

void loop() { 

  int iPattern_Setting = switch_pos(PATTERN_SW_PIN, PATTERN_SW_STEPS, 2);
  int iChase_Len = switch_pos(CHASE_LEN_PIN, CHASE_LEN_STEPS, CHASE_LEN_INCRMNT) + 2;
  int iDelay = 5;
  uint8_t iHue = map(analogRead(H_PIN),0,1023,0,255);
  int iSaturation = map(analogRead(S_PIN),0,1023,0,255);
  int iValue = map(analogRead(V_PIN),0,1023,64,255);
  static uint8_t hue = 0;

  switch (iPattern_Setting) {
    case 1:
      FastLED.showColor (CHSV(hue++, iSaturation, iValue)); 
      iDelay = map(analogRead(DELAY_PIN),0,1023,5,80);
      break;
    case 2:
      FastLED.showColor (CHSV(iHue,iSaturation,iValue));
      break;
    case 3:
      for (int led_ix=0; led_ix < NUM_LEDS; led_ix++) {
        tValue = ((((led_ix + (iChase_Len - chase_offset)) % iChase_Len)) * 255) / 6;
        leds[led_ix] = CHSV(iHue, iSaturation, tValue);
      }
      FastLED.show();
      chase_offset++;
      chase_offset = chase_offset % iChase_Len;
      iDelay = map(analogRead(DELAY_PIN),0,1023,50,200);
      break;
    default:
      FastLED.showColor (CHSV(0,0,0));
      break;
  }
    
  delay(iDelay);
}

int switch_pos(int start_pin, int steps, int step_increment) {
  int iSwitchPos = 0;
  int end_pin = start_pin + ((steps - 1) * step_increment);

  int counter = 0;
  for (int iPin=start_pin; iPin < end_pin; iPin = iPin+step_increment) {
    counter++;
    int switch_state = !digitalRead(iPin);
    if (switch_state == true) {
      iSwitchPos = counter;
    }
  }
  
  return iSwitchPos;
}