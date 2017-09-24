#define SERIAL Serial
#define ESP8266WIFI

// your wifi ssid
const char *SSID = "YOUR-WIFI-SSID";
// your wifi password
const char *WIFI_PASSWORD = "YOUR-WIFI-PASSWORD";

// uncomment to enable REST push to record data
#define ENABLE_ENPOINT_API
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
const unsigned long MOTION_TIME_ON = 1UL;
// timout, in minutes, to stay on after manually turned on via alexa
const unsigned long MANUAL_TIME_ON = 5UL;

// the pin used to trigger the lights
#define TRIGGER_PIN 13
// the pin of the motion sensor
#define PIR_PIN 12