#ifndef WiFiHelper_h
#define WiFiHelper_h
#define ESP8266WIFI
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include "Arduino.h"

class WiFiHelper
{
  public:
    WiFiHelper(const char *ssid, const char *password);
    void check();
    void init();
  private:
    const char *SSID;
    const char *PASSWORD;
};
#endif