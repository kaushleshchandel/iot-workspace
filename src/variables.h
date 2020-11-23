#include <Arduino.h>
#include <config.h>
#include <EEPROM.h>
  
int PM01Value = 0;        //define PM1.0 value of the air detector module
int PM2_5Value = 0;       //define PM2.5 value of the air detector module
int PM10Value = 0;       //define PM10 value of the air detector module
int voc_value = 0; //VOC Sensor 

const char* ssid = DEFAULT_SSID;
const char* password = DEFAULT_WIFI_PWD;

//Device Configurations


/*********************************************************/
/* SAVED CONFIGURATIONS*/

int data_frequency = DEFAULT_SCANNING_FREQUENCY;
int timeZone = DEFAULT_TIME_ZONE;
bool default_unit_C = DEFAULT_UNIT_C;

int blue_distance_max = DEFAULT_MAX_DISTANCE;
int blue_distance_min = DEFAULT_MIN_DISTANCE;
int blue_scan_time = DEFAULT_BLUE_SCANTIME;
int blue_interval = DEFAULT_BLUE_INTERVAL;
bool blue_active_scan = DEFAULT_ACTIVE_SCAN;
int blue_window = DEFAULT_BLUE_WINDOW;
int scan_zone = DEFAULT_SCAN_ZONE_SIZE;
int calibrationRssi = DEFAULT_CALIBRATION_RSSI ;

int spaceCapacity = DEFAULT_SPACE_CAPACITY;
int spaceThreshold = DEFAULT_SPACE_THRESHOLD;
int spaceYellow = DEFAULT_COUNT_TO_YELLOW;
int spaceGreen = DEFAULT_COUNT_TO_GREEN;
int spaceRed = DEFAULT_COUNT_TO_RED;

/*********************************************************/

//MQTT Related Variables
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




void EEPROMWrite(int addr, char *data, int size)
{
  EEPROM.begin(EEPROM_SIZE);
  // write the start marker
  EEPROM.write(addr, EEPROM_START);
  addr++;

  for (int i = 0; i < size; i++)
  {
    EEPROM.write(addr, data[i]);
    addr++;
  }
  EEPROM.write(addr, EEPROM_END);
  EEPROM.commit();
  EEPROM.end();
}

// read bytes from addr util '\0'
// return the length of read out.
int EEPROMread(int addr, char *buf)
{
  EEPROM.begin(EEPROM_SIZE);
  int count = -1;
  char c = EEPROM.read(addr);
  addr++;
  if (c != EEPROM_START)
  {
    return 0;
  }
  while (c != EEPROM_END && count < EEPROM_SIZE)
  {
    c = (char)EEPROM.read(addr);
    count++;
    addr++;
    buf[count] = c;
  }
  EEPROM.end();
  return count;
}


void eeWriteInt(int pos, int val) {
    byte* p = (byte*) &val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}
  

// reset the EEPROM
void clearParam()
{
  char data[EEPROM_SIZE];
  memset(data, '\0', EEPROM_SIZE);
  EEPROMWrite(0, data, EEPROM_SIZE);
}

int spaceCapacityAddr = 0;
int spaceThresholdAddr = 8;
int spaceYellowAddr = 16;
int spaceGreenAddr = 24;
int spaceRedAddr = 32;

int get_space_capacity()
{
  char *totalBoots; 
  totalBoots = (char *)malloc(10);
  EEPROMread(spaceCapacityAddr, totalBoots);
  String sboot = totalBoots;
  return  sboot.toInt(); 

}

void set_space_capacity(int capacity)
{
  char *totalBoots; 
  totalBoots = (char *)malloc(10);
  String sboot = String(capacity+1,DEC);
  sboot.toCharArray(totalBoots, sboot.length()+1 );
  EEPROMWrite(spaceCapacityAddr, totalBoots, strlen(totalBoots));
}