#ifndef LEDHelper_h
#define LEDHelper_h 
#include "FastLED.h"

#define NUM_LEDS 15
#define LED_DATA_PIN 5
#define CHIPSET WS2812B
#define COLOR_ORDER GRB


class LEDHelper {

    
    
    public:
        LEDHelper();
        void init();
        void setColor(byte brightness, CRGB color);
        void lightVisual(CRGB color, CRGB resetColor, int wait, int count);

        const CRGB COLOR_ON = CRGB::White;
        const CRGB COLOR_OFF = CRGB::Black;
        const CRGB COLOR_HTTP_ERROR = CRGB::Red;
        const CRGB COLOR_INIT = CRGB::Purple;
        const CRGB COLOR_WIFI_CONNECTED = CRGB::Blue;
        const CRGB COLOR_WIFI_INIT = CRGB::Orange;
};
#endif