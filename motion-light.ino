/*
 *  Simple HTTP get webclient test
 */
 
 #include "FastLED.h"
 #define SERIAL Serial
 #define ESP8266WIFI
 #ifdef ESP8266WIFI 
   #include <ESP8266WiFi.h>
   #include <ESP8266HTTPClient.h>
 #endif
 
const char* SSID                = "YOUR-WIFI-SSID";
const char* WIFI_PASSWORD       = "YOUR-WIFI-PASSWORD"; 

#define ENABLE_ENPOINT_API
const char* ENDPOINT            = "http://YOUR-NODE-API-SERVER-IP-ADDRESS/api/motion/";
const char* deviceID            = "00000000-0000-0000-0000-000000000000";
const char* device              = "DEVICE-NAME";

const CRGB COLOR_ON             = CRGB::White;
const CRGB COLOR_OFF            = CRGB::Black;
const CRGB COLOR_HTTP_ERROR     = CRGB::Red;
const CRGB COLOR_INIT           = CRGB::Purple;
const CRGB COLOR_WIFI_CONNECTED = CRGB::Blue;
const CRGB COLOR_WIFI_INIT      = CRGB::Orange;

/*********************************** Pin Defintions ********************************/
#define NUM_LEDS      15
#define DATA_PIN       5
#define LED_DATA_PIN  DATA_PIN
#define TRIGGER_PIN   13
#define CLOCK_PIN     TRIGGER_PIN
#define PIR_PIN       12

#define CHIPSET       WS2812B
#define COLOR_ORDER   GRB
struct CRGB leds[NUM_LEDS];
 
CRGB currentColor = COLOR_OFF;
int pirValue = LOW;

void setup() {
    delay(100);
    SERIAL.begin(115200);
    // register LEDs
    FastLED.addLeds<CHIPSET, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.clear();
    // Set the initial color (Black / Off)
    setColor(255,currentColor);

    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);

    digitalWrite(TRIGGER_PIN, HIGH);

    lightVisual(COLOR_INIT, currentColor, 500, 2);
    delay(100);

    #ifdef ESP8266WIFI
    wifi_check();
    #endif
}
  
 
void loop() {
    #ifdef ESP8266WIFI
    wifi_check();
    #endif
    int pirState = digitalRead(PIR_PIN);
    digitalWrite(TRIGGER_PIN, pirState);

    if ( pirState != pirValue ) {
        currentColor = pirState == HIGH ? COLOR_ON : COLOR_OFF;
        setColor(255,currentColor);
            
        SERIAL.print("Changed State: ");
        SERIAL.println(pirState);
        pirValue = pirState;
        #ifdef ENABLE_ENPOINT_API
        record_data(pirState);
        #endif
    } 
}
 
#ifdef ESP8266WIFI
void wifi_check() {
    if (WiFi.status() != WL_CONNECTED) {
        delay(1);
        SERIAL.println("WIFI Disconnected. Attempting reconnection.");
        setup_wifi();
        return;
    }   
}
 
void setup_wifi() {
    delay(1);
    lightVisual(COLOR_WIFI_INIT, currentColor, 100, 3);
    delay(10);
    // We start by connecting to a WiFi network
    SERIAL.println();
    SERIAL.print("Connecting to ");
    SERIAL.println(SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        SERIAL.print(".");
    }

    SERIAL.println("");
    SERIAL.println("WiFi connected.");
    SERIAL.print("IP address: ");
    SERIAL.println(WiFi.localIP());
    delay(1);
    lightVisual(COLOR_WIFI_CONNECTED, currentColor, 100, 3);
}
 
 
#endif
 
void record_data(int state) {
    #ifdef ESP8266WIFI
    HTTPClient http;
    String active = state == LOW ? "true" : "false";
    http.begin(ENDPOINT);
    http.addHeader("X-Auth-Token", deviceID);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST("{\"deviceId\": \"" + String(deviceID) + "\", \"device\": \"" + String(device) + "\", \"state\": " + active + "}");
    http.GET();
    http.writeToStream(&SERIAL);

    if(httpCode > 0) {
        // file found at server
        if(httpCode != HTTP_CODE_CREATED) {
            SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
            lightVisual(COLOR_HTTP_ERROR, currentColor, 100, 3);
        } else {
            SERIAL.printf("[HTTP] POST: %i\n", httpCode);
        }
    } else {
        SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        lightVisual(COLOR_HTTP_ERROR, currentColor, 100, 3);
    }

    http.end();
    #endif
}
 
void setColor(byte brightness, CRGB color) {
    FastLED.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }

    FastLED.setBrightness(brightness);
    FastLED.show();
}
 
void lightVisual(CRGB color, CRGB resetColor, int wait, int count) {
    for (int i = 0; i < count; i++) {
        setColor(255, color);
        delay(wait);
        setColor(255, COLOR_OFF);
        delay(wait);
    }
    setColor(255, resetColor);
}
 
 