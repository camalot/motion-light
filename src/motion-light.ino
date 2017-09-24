
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "Configuration.h"

#include "LEDHelper.h"
#include "OTAHelper.h"
#include "WiFiHelper.h"
#include "CallbackFunction.h"
#include "WemoResponder.h"
#include "Switch.h"
#include "Timer.h"

/******* variables / objects used in rest of the sketch *********/

CRGB currentColor = CRGB::Black;
int pirValue = LOW;
bool manualTrigger = false;

WiFiHelper wifiHelper = WiFiHelper(SSID, WIFI_PASSWORD);
OTAHelper otaHelper = OTAHelper();
LEDHelper ledHelper = LEDHelper();
WemoResponder responder = WemoResponder();
Switch *light = NULL;
Timer shutoffTimer = Timer();

void setup()
{
    delay(100);
    SERIAL.begin(115200);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);
    
    digitalWrite(TRIGGER_PIN, HIGH);
    ledHelper.init();

    ledHelper.lightVisual(ledHelper.COLOR_INIT, currentColor, 500, 2);
    delay(100);
    SERIAL.println("wifi check");
    wifiHelper.check();
    otaHelper.init(DEVICE_NAME, OTA_PASSWORD_HASH);

    responder.beginUdpMulticast();
    light = new Switch(DEVICE_NAME, 80, lightsOn, lightsOff);
    Serial.println("Adding switches upnp broadcast responder");
    responder.addDevice(*light);
}

void lightsOn() {
    SERIAL.println("Manual Override: ON");
    manualTrigger = true;
    shutoffTimer.every_n_minutes(MANUAL_TIME_ON);
}

void lightsOff() {
    SERIAL.println("Changed State: OFF");
    manualTrigger = false;
    shutoffTimer.stop();
    ledHelper.setColor(255, ledHelper.COLOR_OFF);

    #ifdef ENABLE_ENPOINT_API
        record_data(ENDPOINT, DEVICE_ID, DEVICE_NAME, LOW);
    #endif
}

void loop()
{
    wifiHelper.check();
    responder.serverLoop();
    light->serverLoop();

    delay(1);
    otaHelper.check();
    
    int realPIRState = digitalRead(PIR_PIN);
    unsigned long timeout = manualTrigger ? MANUAL_TIME_ON : MOTION_TIME_ON;
    
    if(shutoffTimer.isRunning()) {
        shutoffTimer.check(lightsOff);

        if ( realPIRState == HIGH ) {
            // reset timer
            shutoffTimer.every_n_minutes(timeout);
        }
    
    }

    int pirState = manualTrigger || shutoffTimer.isRunning() ? HIGH : realPIRState;
    
    digitalWrite(TRIGGER_PIN, pirState);

    
    // off only happens after timeout of timer
    if (pirValue != pirState && pirState == HIGH) {
        SERIAL.println("Changed State: ON");
        shutoffTimer.every_n_minutes(timeout);
        ledHelper.setColor(255, ledHelper.COLOR_ON);
        pirValue = pirState;

        #ifdef ENABLE_ENPOINT_API
            record_data(ENDPOINT, DEVICE_ID, DEVICE_NAME, pirState);
        #endif
    } else if (pirValue != pirState && pirState == LOW) {
        pirValue = pirState;
    }
}

void record_data(const char* endpoint, const char* deviceId, const char* deviceName, int state) {
#ifdef ESP8266WIFI
    HTTPClient http;
    String active = state == HIGH ? "true" : "false";
    String motion_triggered = !manualTrigger ? "true" : "false";
    http.begin(endpoint);
    http.addHeader("X-Auth-Token", deviceId);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST("{\"device_id\": \"" + String(deviceId) + "\", \"device\": \"" + String(deviceName) + "\", \"active\": " + active + ", \"motion_triggered\": " + motion_triggered + "}");
    http.GET();
    http.writeToStream(&SERIAL);

    if (httpCode > 0) {
        // file found at server
        if (httpCode != HTTP_CODE_CREATED) {
            SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
            ledHelper.lightVisual(ledHelper.COLOR_HTTP_ERROR, currentColor, 100, 3);
        } else {
            SERIAL.printf("[HTTP] POST: %i\n", httpCode);
        }
    } else {
        SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        ledHelper.lightVisual(ledHelper.COLOR_HTTP_ERROR, currentColor, 100, 3);
    }

    http.end();
#endif
}
