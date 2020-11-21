
#define DEVICE_HW_OCC    //Use this for Code selection
#define SW_VERSION "0.0" //Change with every build

#include <PubSubClient.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <variables.h>
#include <ota.h>
#include <mqtt.h>
#include <m5hats.h>
#include <fx.h>

#if defined(DEVICE_HW_OCC) //Occupancy Counter
#define HW_VERSION HW_OCC
#include <M5StickC.h>
#include <display.h>
#elif defined(DEVICE_HW_AQM) //Air Quality
#define HW_VERSION HW_AQM
#include <air_qaulity_sensors.h>
#endif

void setup()
{
  Serial.begin(115200);
  delay(10);

#if defined(DEVICE_HW_OCC) //Occupancy Counter
  Serial.println("Device Mode: Occupancy");
  M5.begin();

  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 40);
startupScreen();
#elif defined(DEVICE_HW_AQM) //Air Quality
  Serial1.begin(9600)
      Serial.println("Device Mode: Occupancy Trakcer");
#endif

  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  checkUpdateFirmware(SW_VERSION, HW_VERSION);

  // Printing the ESP IP address
  Serial.println(WiFi.localIP());

  init_mqtt(SW_VERSION, HW_VERSION);

  //Add Code here to check for hats

  sendConfig(HW_VERSION);

#if defined(DEVICE_HW_OCC) //Occupancy Counter
  //Add Code for Initializing the Occupanc Sensors & the HATS
#elif defined(DEVICE_HW_AQM) //Air Quality
  init_sensors();

#endif
}

void loop()
{

  loopsPM++;
  unsigned long currentMillis = millis();

#if defined(DEVICE_HW_OCC) //Occupancy Counter

#elif defined(DEVICE_HW_AQM) //Air Quality
  read_pm_sensor(PM01Value, PM2_5Value, PM10Value);
  read_voc_sensor(voc_value);
#endif

  //Based on data frequency, send the data
  //data_frequency
  //Send Diagnostics data every 1 minute seconds
  if (currentMillis - previousDataMillis >= data_frequency * 1000)
  {
    dataLoopsPM++;
    previousDataMillis = currentMillis;

#if defined(DEVICE_HW_OCC) //Occupancy Counter

#elif defined(DEVICE_HW_AQM) //Air Quality
    send_mqtt_int("data/pm1.0", PM01Value, false);
    send_mqtt_int("data/pm2.5", PM2_5Value, false);
    send_mqtt_int("data/pm10.", PM10Value, false);
    send_mqtt_int("data/voc", voc_value, false);
#endif
  }


  checkHealthStatus();

  if (currentMillis - previousDiagnosticsMillis >= 60 * 1000)
  {
    previousDiagnosticsMillis = currentMillis;
    sendDiagnosticsData(mqttclient);
  }
}