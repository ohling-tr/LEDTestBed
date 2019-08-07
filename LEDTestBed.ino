#include <elapsedMillis.h>

#include "Adafruit_LiquidCrystal.h"

#include "FastLED.h"
//#include "LiquidCrystal_I2C.h"
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
uint8_t tValue = 0;
uint8_t tHue = 0;

elapsedMillis LED_Delay;
elapsedMillis LCD_Delay;

Adafruit_LiquidCrystal lcd(0);
//LiquidCrystal_I2C lcd(0x27,16,2);

char line0[17];
char line1[17];

void setup() {
   // FastLED.addLeds<NEOPIXEL, 6>(leds, 60); 
   FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);

   for (int iPin=22; iPin < 53; iPin++) {
     pinMode(iPin, INPUT_PULLUP);
   }

   //Serial.begin(9600);
   lcd.begin(16,2);
   //lcd.init();
   lcd.setBacklight(HIGH);
   LED_Delay = 0;
   LCD_Delay = 0;
}

void loop() { 

  int iPattern_Setting = switch_pos(PATTERN_SW_PIN, PATTERN_SW_STEPS, 2);
  int iChase_Len = switch_pos(CHASE_LEN_PIN, CHASE_LEN_STEPS, CHASE_LEN_INCRMNT) + 2;
  //int iDelay = 5;
  int iDelay = map(analogRead(DELAY_PIN),0,1023,100,10);
  uint8_t iHue = map(analogRead(H_PIN),0,1023,0,255);
  uint8_t iRed = iHue;
  uint8_t iSaturation = map(analogRead(S_PIN),0,1023,0,255);
  uint8_t iHue2 = iSaturation;
  uint8_t iGreen = iSaturation;
  uint8_t iValue = map(analogRead(V_PIN),0,1023,64,255);
  uint8_t iBlue = map(analogRead(V_PIN),0,1023,0,255);
  static uint8_t hue = 0;

  if (LED_Delay >= iDelay) {
    switch (iPattern_Setting) {
      case 1:
        FastLED.showColor (CHSV(hue++, iSaturation, iValue)); 

        sprintf(line0,"Hue Rainbow %3u ", iDelay);
        sprintf(line1,"Hnnn S%3u V%3u ", iSaturation, iValue);
        break;
      case 2:
        FastLED.showColor (CHSV(iHue,iSaturation,iValue));
        sprintf(line0,"Hue/Sat/Val HSV ");
        sprintf(line1,"H%3u S%3u V%3u ", iHue, iSaturation, iValue);
        break;
      case 3:
        for (int led_ix=0; led_ix < NUM_LEDS; led_ix++) {
          tValue = ((((led_ix + (iChase_Len - chase_offset)) % iChase_Len)) * iValue) / iChase_Len;
          leds[led_ix] = CHSV(iHue, iSaturation, tValue);
        }
        FastLED.show();
        chase_offset++;
        chase_offset = chase_offset % iChase_Len;
        sprintf(line0,"HSV Chase %1u %3u ", iChase_Len, iDelay);
        sprintf(line1,"H%3u S%3u V%3u ", iHue, iSaturation, iValue);
        break;
      case 4:
        for (int led_ix = 0; led_ix < NUM_LEDS; led_ix ++) {
          if (iHue < iHue2) {
            tHue = (((((led_ix + (iChase_Len - chase_offset)) % iChase_Len)) * (iHue2 - iHue)) / iChase_Len) + iHue;
          } else {
            tHue = (((((led_ix + (iChase_Len - chase_offset)) % iChase_Len)) * (iHue - iHue2)) / iChase_Len) + iHue2;
          }
          leds[led_ix] = CHSV(tHue, 255, iValue);
        }
        FastLED.show();
        chase_offset++;
        chase_offset = chase_offset % iChase_Len;
        sprintf(line0,"Hue Chase %1u %3u ", iChase_Len, iDelay);
        sprintf(line1,"H%3u H%3u V%3u ", iHue, iHue2, iValue);
        break;
      case 5:
        for (int led_ix=0; led_ix < NUM_LEDS; led_ix++) {
          leds[led_ix].setRGB(iRed, iGreen, iBlue);
        }
        FastLED.show();
        sprintf(line0,"Red/Green/Blue  ");
        sprintf(line1,"R%3u G%3u B%3u ", iRed, iGreen, iBlue);
        break;
      default:
        FastLED.showColor (CHSV(0,0,0));
        sprintf(line0,"Black           ");
        sprintf(line1,"aka OFF         ");
        break;
    }
    LED_Delay = 0;
  }

  if (LCD_Delay >= 500) {
    LCD_Delay = 0;
    lcd.setCursor(0,0);
    lcd.print(line0);
    lcd.setCursor(0,1);
    lcd.print(line1);
  }
    
//  delay(iDelay);
}

int switch_pos(int start_pin, int steps, int step_increment) {
  int iSwitchPos = 0;
  int end_pin = start_pin + ((steps - 1) * step_increment);

  int counter = 0;
  for (int iPin=start_pin; iPin <= end_pin; iPin = iPin+step_increment) {
    counter++;
    int switch_state = !digitalRead(iPin);
    if (switch_state == true) {
      iSwitchPos = counter;
    }
  }
  
  return iSwitchPos;
}