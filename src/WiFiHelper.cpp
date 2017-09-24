#include "WiFiHelper.h"
#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

WiFiHelper::WiFiHelper(const char *ssid, const char *password)
{
    SSID = ssid;
    PASSWORD = password;
}

void WiFiHelper::init()
{
    delay(1);
    //lightVisual(COLOR_WIFI_INIT, currentColor, 100, 3);
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.println("Connection Failed! Rebooting...");
    //     delay(5000);
    //     ESP.restart();
    // }
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    delay(1);
    //lightVisual(COLOR_WIFI_CONNECTED, currentColor, 100, 3);
}

void WiFiHelper::check()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        delay(1);
        Serial.println("WIFI Disconnected. Attempting reconnection.");
        WiFiHelper::init();
        return;
    } 
    
    return;
}


