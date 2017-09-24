#ifndef OTAHelper_h
#define OTAHelper_h

#include "Arduino.h"
#include "ArduinoOTA.h"

class OTAHelper
{
  public:
    OTAHelper();
    void init(const char *deviceName, const char *passwordHash);
    void check();

  private:
};
#endif