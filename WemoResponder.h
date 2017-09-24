#ifndef WemoResponder_h
#define WemoResponder_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "Switch.h"

class WemoResponder
{
  private:
    WiFiUDP UDP;

  public:
    WemoResponder();
    ~WemoResponder();
    bool beginUdpMulticast();
    void serverLoop();
    void addDevice(Switch &device);
};

#endif