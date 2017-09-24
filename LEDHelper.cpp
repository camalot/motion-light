#include "LEDHelper.h"
#include "Arduino.h"
#include "FastLED.h"

struct CRGB leds[NUM_LEDS];

LEDHelper::LEDHelper() {

}

void LEDHelper::init() {
    Serial.println("init led helper");
    // register LEDs
    FastLED.addLeds<CHIPSET, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();
    // Set the initial color (Black / Off)
    LEDHelper::setColor(255, CRGB::Black);
    
}

void LEDHelper::setColor(byte brightness, CRGB color) {
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
}

void LEDHelper::lightVisual(CRGB color, CRGB resetColor, int wait, int count) {
    for (int i = 0; i < count; i++) {
        LEDHelper::setColor(255, color);
        delay(wait);
        LEDHelper::setColor(255, CRGB::Black);
        delay(wait);
    }
    LEDHelper::setColor(255, resetColor);
}