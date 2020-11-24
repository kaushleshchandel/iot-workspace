#include <Arduino.h>
#include <config.h>
#include <EEPROM.h>

int PM01Value = 0;  //define PM1.0 value of the air detector module
int PM2_5Value = 0; //define PM2.5 value of the air detector module
int PM10Value = 0;  //define PM10 value of the air detector module
int voc_value = 0;  //VOC Sensor

const char *ssid = DEFAULT_SSID;
const char *password = DEFAULT_WIFI_PWD;
//Device Configurations

/*********************************************************/
/* SAVED CONFIGURATIONS*/

int data_frequency = DEFAULT_SCANNING_FREQUENCY;
int timeZone = DEFAULT_TIME_ZONE;
int default_unit_C = DEFAULT_UNIT_C;

int blue_distance_max = DEFAULT_MAX_DISTANCE;
int blue_distance_min = DEFAULT_MIN_DISTANCE;
int blue_scan_time = DEFAULT_BLUE_SCANTIME;
int blue_interval = DEFAULT_BLUE_INTERVAL;
bool blue_active_scan = DEFAULT_ACTIVE_SCAN;
int blue_window = DEFAULT_BLUE_WINDOW;
int scan_zone = DEFAULT_SCAN_ZONE_SIZE;
int calibrationRssi = DEFAULT_CALIBRATION_RSSI;

int spaceCapacity = DEFAULT_SPACE_CAPACITY;
int spaceThreshold = DEFAULT_SPACE_THRESHOLD;
int spaceYellow = DEFAULT_COUNT_TO_YELLOW;
int spaceGreen = DEFAULT_COUNT_TO_GREEN;
int spaceRed = DEFAULT_COUNT_TO_RED;

/*********************************************************/

//MQTT Related Variables
const char *mqtt_server = DEFAULT_MQTT_SERVER;
int default_mqtt_port = DEFAULT_MQTT_PORT;
const char *mqtt_user = DEFAULT_MQTT_USER;
const char *mqtt_pass = DEFAULT_MQTT_PASSWORD;
const char *mqtt_topic_prefix = DEFAULT_TOPIC_PREFIX;
const char *mqtt_subs_topic = DEFAULT_SUB_TOPIC;

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

//for EEPROM
int spaceCapacityAddr = 0;
int dataFrequencyAddr = 10;
int spaceYellowAddr = 20;
int spaceGreenAddr = 30;
int spaceRedAddr = 40;
int spaceThresholdAddr = 50;
int unitCelciusAddr = 60;
int scantimeAddr = 70;
int scanIntervalAddr = 80;
int scanWindowAddr = 90;
//int scanWindowAddr = 100;
int calibrationRssiAddr = 110;
int activeScanAddr = 120;
int maxDistanceAddr = 130;
int minDistanceAddr = 140;
int deviceIDAddr = 150;

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

void eeWriteInt(int pos, int val)
{
    Serial.printf("Writing %d at %d \n", val, pos);

    EEPROM.begin(EEPROM_SIZE);
    byte *p = (byte *)&val;
    EEPROM.write(pos, *p);
    EEPROM.write(pos + 1, *(p + 1));
    EEPROM.write(pos + 2, *(p + 2));
    EEPROM.write(pos + 3, *(p + 3));
    EEPROM.commit();
    EEPROM.end();
}

int eeGetInt(int pos)
{
    int val;
    byte *p = (byte *)&val;
     EEPROM.begin(EEPROM_SIZE);
    *p = EEPROM.read(pos);
    *(p + 1) = EEPROM.read(pos + 1);
    *(p + 2) = EEPROM.read(pos + 2);
    *(p + 3) = EEPROM.read(pos + 3);
    EEPROM.end();
    Serial.printf("Read %d at %d \n", val, pos);
    return val;
}

// reset the EEPROM
void clearParam()
{
    char data[EEPROM_SIZE];
    EEPROM.begin(EEPROM_SIZE);
    memset(data, '\0', EEPROM_SIZE);
    EEPROMWrite(0, data, EEPROM_SIZE);
    EEPROM.commit();
    EEPROM.end();
}

void save_config()
{
    Serial.println("Saving config");

    eeWriteInt(dataFrequencyAddr, data_frequency);

    eeWriteInt(activeScanAddr, blue_active_scan);
    eeWriteInt(maxDistanceAddr, blue_distance_max);
    eeWriteInt(minDistanceAddr, blue_distance_min);
    eeWriteInt(scantimeAddr, blue_scan_time);
    eeWriteInt(scanIntervalAddr, blue_interval);
    eeWriteInt(scanWindowAddr, blue_window);
    eeWriteInt(spaceCapacityAddr, spaceCapacity);
    eeWriteInt(dataFrequencyAddr, data_frequency);
    eeWriteInt(spaceYellowAddr, spaceYellow);
    eeWriteInt(spaceGreenAddr, spaceGreen);
    eeWriteInt(spaceRedAddr, spaceRed);
    eeWriteInt(spaceThresholdAddr, spaceThreshold);
    eeWriteInt(calibrationRssiAddr, calibrationRssi);
}

void set_default_config()
{
    Serial.println("Setting default config");
    data_frequency = DEFAULT_SCANNING_FREQUENCY;
    timeZone = DEFAULT_TIME_ZONE;
    default_unit_C = DEFAULT_UNIT_C;

    blue_distance_max = DEFAULT_MAX_DISTANCE;
    blue_distance_min = DEFAULT_MIN_DISTANCE;
    blue_scan_time = DEFAULT_BLUE_SCANTIME;
    blue_interval = DEFAULT_BLUE_INTERVAL;
    blue_window = DEFAULT_BLUE_WINDOW;
    //  blue_active_scan= DEFAULT_BLUE_ACTIVE_SCAN;
    spaceCapacity = DEFAULT_SPACE_CAPACITY;
    spaceThreshold = DEFAULT_SPACE_THRESHOLD;
    spaceYellow = DEFAULT_COUNT_TO_YELLOW;
    spaceGreen = DEFAULT_COUNT_TO_GREEN;
    spaceRed = DEFAULT_COUNT_TO_RED;
}

bool read_config()
{
    Serial.println("Reading config");

   Serial.print("Scan Time ");  blue_scan_time = eeGetInt(scantimeAddr);
   Serial.print("Interval  ");  blue_interval = eeGetInt(scanIntervalAddr);
   Serial.print("Window    ");  blue_window = eeGetInt(scanWindowAddr);
   Serial.print("Capacity  ");  spaceCapacity = eeGetInt(spaceCapacityAddr);
   Serial.print("Frequency ");  data_frequency = eeGetInt(dataFrequencyAddr);
   Serial.print("Temp Celc ");  default_unit_C = eeGetInt(unitCelciusAddr);
   Serial.print("Yellow    ");  spaceYellow = eeGetInt(spaceYellowAddr);
   Serial.print("Green     ");  spaceGreen = eeGetInt(spaceGreenAddr);
   Serial.print("Red       ");  spaceRed = eeGetInt(spaceRedAddr);
   Serial.print("Threshold ");  spaceThreshold = eeGetInt(spaceThresholdAddr);
   Serial.print("Cali RSSI ");  calibrationRssi = eeGetInt(calibrationRssiAddr);
   Serial.print("Active Scn ");  blue_active_scan = eeGetInt(activeScanAddr);
   Serial.print("Max Dist  ");  blue_distance_max = eeGetInt(maxDistanceAddr);
   Serial.print("Min Dist  ");  blue_distance_min = eeGetInt(minDistanceAddr);

    if (blue_scan_time > 0 && blue_interval > 0 && blue_window > 0)
    {
        Serial.println("Config Read OK");
        return true;
    }
    else
    {
        return false;
    }
}
