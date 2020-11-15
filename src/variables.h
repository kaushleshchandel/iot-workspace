#include <Arduino.h>
#include <config.h>

  
int PM01Value = 0;        //define PM1.0 value of the air detector module
int PM2_5Value = 0;       //define PM2.5 value of the air detector module
int PM10Value = 0;       //define PM10 value of the air detector module
int voc_value = 0; //VOC Sensor 
float temperature_value;
float pressure_value;
float humidity_value;
float gas_resistance_value; 
float altitude_value;


const char* ssid = DEFAULT_SSID;
const char* password = DEFAULT_WIFI_PWD;

int data_frequency = DEFAULT_SCANNING_FREQUENCY;
int blue_distance_max = DEFAULT_BLUE_RSSI_LIMIT_HIGH;
int blue_distance_min = DEFAULT_BLUE_RSSI_LIMIT_LOW;
int blue_scan_time = DEFAULT_BLUE_SCANTIME;
int blue_interval = DEFAULT_BLUE_INTERVAL;
int blue_window = DEFAULT_BLUE_WINDOW;
int timeZone = DEFAULT_TIME_ZONE;
bool default_unit_C = DEFAULT_UNIT_C;
int scan_zone = DEFAULT_SCAN_ZONE_SIZE;
int calibrationRssi = DEFAULT_CALIBRATION_RSSI ;

const char *mqtt_server = DEFAULT_MQTT_SERVER;
const char *mqtt_user = DEFAULT_MQTT_USER;
const char *mqtt_pass = DEFAULT_MQTT_PASSWORD;
const char* mqtt_topic_prefix = DEFAULT_TOPIC_PREFIX;
const char* mqtt_subs_topic = DEFAULT_SUB_TOPIC;

//Coutners for Timers
int previousDiagnosticsMillis = 0;
int previousDataMillis;
int previousSystemMillis;


//Counters for diagnositcs
unsigned long packetsSentPM = 0;
unsigned long packetsFailPM = 0;
unsigned long bytesSentPM = 0;
unsigned long dataLoopsPM = 0;
unsigned long loopsPM = 0;
unsigned long lastpacketsSentPM = 1;

unsigned long wifiErrors = 0;
unsigned long mqttErrors = 0;
unsigned long sensorReadErrors = 0;
unsigned long bleErrors = 0;