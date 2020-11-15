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
#include <air_qaulity_sensors.h>
#include <thingspeak.h>

#include <fx.h>

#define SW_VERSION "1.0"  //Change with every build
#define HW_VERSION HW_AQM //Change for hardware

WiFiClient client;

PubSubClient mqttclient(client);

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
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

  init_mqtt(mqttclient, SW_VERSION, HW_VERSION);
  sendConfig(mqttclient, HW_VERSION);

  init_sensors();
}

void loop()
{

  loopsPM++;
  read_pm_sensor(PM01Value, PM2_5Value, PM10Value);

  read_voc_sensor(voc_value);
 
  read_bme680_sensor(temperature_value, pressure_value, humidity_value, gas_resistance_value, altitude_value);

  checkHealthStatus();


  //Based on data frequency, send the data
  //data_frequency
  //Send Diagnostics data every 1 minute seconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousDataMillis >= data_frequency * 1000)
  {
    dataLoopsPM++;
    previousDataMillis = currentMillis;

    send_mqtt_int(mqttclient, "data/pm1.0", PM01Value, false);
    send_mqtt_int(mqttclient, "data/pm2.5", PM2_5Value, false);
    send_mqtt_int(mqttclient, "data/pm10.", PM10Value, false);
    send_mqtt_int(mqttclient, "data/voc", voc_value, false);
  }

  //Send diagnostics data
  if (currentMillis - previousDiagnosticsMillis >= 60 * 1000)
  {
    previousDiagnosticsMillis = currentMillis;
    sendDiagnosticsData(mqttclient);
  }

 
}