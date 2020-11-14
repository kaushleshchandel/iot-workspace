#include <Arduino.h>
#include <config.h>

/*Define the Constants */

  
int PM01Value = 0;        //define PM1.0 value of the air detector module
int PM2_5Value = 0;       //define PM2.5 value of the air detector module
int PM10Value = 0;       //define PM10 value of the air detector module


/* CHANGE BELOW THIS LINE AS PER DESIGN */
String apiKey = THING_SPEAK_API_KEY;
const char* ssid = DEFAULT_SSID;
const char* password = DEFAULT_WIFI_PWD;
const char* server = THING_SPEAK_SERVER;

