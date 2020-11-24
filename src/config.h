
//Define all the Constants here
//This library should not be used directly in the code
// Used by the Variables.h to create variables usign teh default values

#include <Arduino.h>

/* All the COnstants defined as ALL CAPS*/
#define DEFAULT_SSID  "AAA"
#define DEFAULT_WIFI_PWD "608980608980"
#define DEFAULT_MQTT_SERVER "34.71.63.201"
#define DEFAULT_MQTT_PORT 1883 
#define DEFAULT_MQTT_USER "m5-beacon"
#define DEFAULT_MQTT_PASSWORD "hf*!EYfk4jQQjfG9nGf7SHQbJ5"
#define DEFAULT_MESSAGE_LENGTH 50


#define DEFAULT_ACTIVE_SCAN  true
#define DEFAULT_MIN_DISTANCE 0 // Default Minimam Distance for counting  
#define DEFAULT_MAX_DISTANCE 10 // Default highest RSSI to send data for
#define DEFAULT_SCANNING_FREQUENCY  10// 30 // Data sending frequency in seconds
#define DEFAULT_BLUE_SCANTIME  5 // BLE Scantime
#define DEFAULT_BLUE_INTERVAL  100 // BLE Scantime
#define DEFAULT_BLUE_WINDOW  99 // BLE Scan Window 


#define  DEFAULT_SPACE_THRESHOLD 0.7 //Percentage at which it turns Yellow
#define  DEFAULT_SPACE_CAPACITY 3 //Total number of occupants 
#define  DEFAULT_COUNT_TO_YELLOW 2 // Count at which it turns yellow
#define  DEFAULT_COUNT_TO_GREEN 1 //Count at which it shows Green
#define  DEFAULT_COUNT_TO_RED 3 //Count at which it shows Red

#define DEFAULT_TIME_ZONE 0000
#define DEFAULT_UNIT_C 0

#define DEFAULT_TOPIC_PREFIX ""
#define DEFAULT_GLOBAL_SUB_TOPIC "secure"
#define DEFAULT_SUB_TOPIC "set"

#define DEFAULT_SCAN_ZONE_SIZE 5

//BLE RELATED
#define TRACKING_DISTANCE_THRASHHOLD 5  //RSSI Thrashold to consider a change 
#define DEFAULT_CALIBRATION_RSSI  -75 //Use the RSSI for 1 Meter distance
 

//hardware Versions
#define HW_AQM "ESP32-AQM"
#define HW_OCC "M5STICK-C"

#define EEPROM_END 0
#define EEPROM_START 1
#define EEPROM_SIZE 512