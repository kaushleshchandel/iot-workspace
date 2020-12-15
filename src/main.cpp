
#define DEVICE_HW_OCC    //Use this for Code selection
#define SW_VERSION "1.7" //Change with every build

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
#include <fx.h>

#if defined(DEVICE_HW_OCC) //Occupancy Counter
#define HW_VERSION HW_OCC
#include <ble_counter.h>
#include <M5StickC.h>
#include <display.h>
#include <m5hats.h>
#include <captiv.h>

BLE_counter BLE_counter;

#elif defined(DEVICE_HW_AQM) //Air Quality
#define HW_VERSION HW_AQM
#include <air_qaulity_sensors.h>
#endif

bool newRun = true;

void setup()
{
  Serial.begin(115200);
  delay(100);

  if (read_config() == false)
  {
    Serial.println("Blank Config. Setting defaults");
    set_default_config();
    save_config();
  }

#if defined(DEVICE_HW_OCC) //Occupancy Counter
  Serial.println("Device Mode: Occupancy");
  M5.begin();
  initButtons();
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 40);
  startupScreen(SW_VERSION);
  delay(1000);
  //spaceCapacity = get_space_capacity();

  BLE_counter.init(true, blue_interval, blue_window, blue_distance_max, blue_distance_min, blue_scan_time, blue_window, calibrateRssi);
#elif defined(DEVICE_HW_AQM) //Air Quality
  Serial1.begin(9600)
      Serial.println("Device Mode: Occupancy Trakcer");
#endif

  //WiFi.begin(ESP_wifiManager.WiFi_SSID(), ESP_wifiManager.WiFi_Pass())

  wm_setup(false); //Start WifiManager

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
  if (currentMillis - previousDataMillis >= data_frequency * 1000 || newRun == true)
  {
    dataLoopsPM++;
    previousDataMillis = currentMillis;

#if defined(DEVICE_HW_OCC) //Occupancy Counter
    int occup = 0;
    int totl = 0;
    BLE_counter.get_count(totl, occup);
    send_mqtt_int("data/occp", occup, false);
    send_mqtt_int("data/totl", totl, false);

    int ocolor = 0;
    if (occup >= spaceRed)
      ocolor = 3;
    else if (occup >= spaceYellow)
      ocolor = 2;
    else if (occup >= spaceGreen)
      ocolor = 1;
    else
      ocolor = 0;
    display_occupancy(occup, ocolor);
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

  mqttclient.loop();
  newRun = false;

  if (buttonhome.pressed)
  {
    buttonhome.pressed = false;
  }

  if (buttonside.pressed)
  {
    buttonside.pressed = false;
  }

  if (buttonside.numberKeyPresses >= 10)
  { 
   // wm_loop(true);
    wm_loop(true); //Start WifiManagre
    buttonside.numberKeyPresses = 0;
  }
  
  wm_loop(false);

}