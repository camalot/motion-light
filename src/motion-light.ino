/*
 *  Simple HTTP get webclient test
 */

#define SERIAL Serial
#include <ESP8266HTTPClient.h>
#define ESP8266WIFI
#include "LEDHelper.h"
#include "OTAHelper.h"
#include "WiFiHelper.h"
#include "CallbackFunction.h"
#include "WemoResponder.h"
#include "Switch.h"
#include "Timer.h"

// your wifi ssid
const char *SSID = "YOUR-WIFI-SSID";
// your wifi password
const char *WIFI_PASSWORD = "YOUR-WIFI-PASSWORD";

// uncomment to enable REST push to record data
// #define ENABLE_ENPOINT_API
// this is the REST endpoint to push the json object
const char *ENDPOINT = "http://YOUR-NODE-API-SERVER-IP-ADDRESS/api/motion/";
// a unique guid for this device
const char *deviceID = "00000000-0000-0000-0000-000000000000";
// a unique name for the device
const char *device = "DEVICE-NAME";

// you set this to a hashed password, then you can push code updates over wifi
// https://www.md5hashgenerator.com/
const char *OTA_PASSWORD_HASH = "MD5_HASHED_PASSWORD";

// timeout, in minutes, to stay on after a motion is triggered
const unsigned long MOTION_TIME_ON = 2;
// timout, in minutes, to stay on after manually turned on via alexa
const unsigned long MANUAL_TIME_ON = 5;

#define TRIGGER_PIN 13
#define PIR_PIN 12

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

void lightsOn()
{
    SERIAL.println("Manual Override: ON");
    manualTrigger = true;
    shutoffTimer.every_n_minutes(MANUAL_TIME_ON);
}

void lightsOff()
{
    SERIAL.println("Manual Override: OFF");
    manualTrigger = false;
    shutoffTimer.stop();
}

void loop()
{
    wifiHelper.check();
    responder.serverLoop();
    light->serverLoop();

    delay(1);
    otaHelper.check();
    int pirState = manualTrigger ? HIGH : digitalRead(PIR_PIN);
    digitalWrite(TRIGGER_PIN, pirState);

    if (shutoffTimer.isRunning())
    {
        shutoffTimer.check(lightsOff);
    }
    if (pirState != pirValue)
    {
        currentColor = pirState == HIGH ? ledHelper.COLOR_ON : ledHelper.COLOR_OFF;
        unsigned long timeout = manualTrigger ? MANUAL_TIME_ON : MOTION_TIME_ON;
        shutoffTimer.every_n_minutes(timeout);
        ledHelper.setColor(255, currentColor);

        SERIAL.print("Changed State: ");
        SERIAL.println(pirState);
        pirValue = pirState;
#ifdef ENABLE_ENPOINT_API
        record_data(ENDPOINT, DEVICE_ID, DEVICE_NAME, pirState);
#endif
    }
}

void record_data(const char *endpoint, const char *deviceId, const char *deviceName, int state)
{
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

    if (httpCode > 0)
    {
        // file found at server
        if (httpCode != HTTP_CODE_CREATED)
        {
            SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
            ledHelper.lightVisual(ledHelper.COLOR_HTTP_ERROR, currentColor, 100, 3);
        }
        else
        {
            SERIAL.printf("[HTTP] POST: %i\n", httpCode);
        }
    }
    else
    {
        SERIAL.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
        ledHelper.lightVisual(ledHelper.COLOR_HTTP_ERROR, currentColor, 100, 3);
    }

    http.end();
#endif
}
