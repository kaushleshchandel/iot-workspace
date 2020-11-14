#include <PubSubClient.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <variables.h>
#include <fx.h>
#include <air_qaulity_sensors.h>
#include <thingspeak.h>
#include <ota.h>
#include <mqtt.h>

#define SW_VERSION  "1.0" //Change with every build
#define HW_VERSION  "ESP32-AIR" //Change for hardware

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
  init_mqtt( mqttclient, "1.0", "ESP");
}

void loop()
{

  if (read_pm_sensor(PM01Value, PM2_5Value, PM10Value))
  {
    //
  }

  read_voc_sensor(AIRVOC);


  static unsigned long OledTimer = millis();
  if (millis() - OledTimer >= 1000)
  {
    OledTimer = millis();

    Serial.print("PM1.0: ");
    Serial.print(PM01Value);
    Serial.println("  ug/m3");

    Serial.print("PM2.5: ");
    Serial.print(PM2_5Value);
    Serial.println("  ug/m3");

    Serial.print("PM10 : ");
    Serial.print(PM10Value);
    Serial.println("  ug/m3");
    Serial.println();

    Serial.print("VOC : ");
    Serial.print(AIRVOC);
    Serial.println(" ppm");
    Serial.println();

    if (WiFi.isConnected() != true)
    {
      Serial.println("Wifi Not Connected");
    }
    else
    {
       send_mqtt_int(mqttclient, "data/pm1.0", PM01Value, false);
       send_mqtt_int(mqttclient, "data/pm2.5", PM2_5Value, false);
       send_mqtt_int(mqttclient, "data/pm10.", PM10Value, false);
       send_mqtt_int(mqttclient, "data/voc", AIRVOC, false); 
    }
  }
}