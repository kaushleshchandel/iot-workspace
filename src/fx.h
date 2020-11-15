/* 
    Only define Fully self contained functions
    DO NOT USE ANY CONSTANTS OR VARIABLS
    IF needed, pass as

 */


//Tries to reconnect to wifi
void reconnect_WiFi()
{
    WiFi.disconnect();
    Serial.println("Reconnecting to WiFi...");
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);

   //Try to reconnect for 5 minutes
    for (int i = 0; i < 60*5 ; i++)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.print(".");
        }
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.println("WiFi Connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void sendDiagnosticsData(PubSubClient &client)
{
 // Update this to a Json structure
  double uptime = esp_timer_get_time() / 3600000000; // Hours
  //double uptime = esp_timer_get_time() / (1000000*60*60) ;// Hours

  send_mqtt_int(client, "system/loop-k", loopsPM / 1000, false );
  send_mqtt_int(client, "system/reads", dataLoopsPM, false );
  send_mqtt_int(client, "system/pkt-sent", packetsSentPM, false );
  send_mqtt_int(client, "system/mem-kb", ESP.getFreeHeap()/1024, false );
  send_mqtt_int(client, "system/fail", packetsFailPM, false );
  send_mqtt_int(client, "system/wifi-err", wifiErrors, false );
  send_mqtt_int(client, "system/mqtt-err", mqttErrors , false);
  send_mqtt_int(client, "system/ble-err", bleErrors, false );
  send_mqtt_float(client, "system/uptime-hrs", uptime, false );

  loopsPM = 0;
  dataLoopsPM = 0;
  lastpacketsSentPM = packetsSentPM;
  packetsSentPM = 0;
  packetsFailPM = 0;
  bytesSentPM = 0;

}

//Checks and reconnects Wifi
void checkHealthStatus()
{
    if ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print("Wifi Disconnected ");
        reconnect_WiFi();
    }

    //Check again. if not connected, then reboot
    if ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print("Wifi Connectivity Failure ");
        Serial.print("Reboot ");
        delay(1000);
        esp_restart();
    }


}

//Send device Configurations
void sendConfig(PubSubClient &client, String hw_version) {
  // Update this to a Json structure

  send_mqtt_int(client, "config/dataFrequency", data_frequency, false );
  send_mqtt_int(client, "config/unitCelsius", default_unit_C, false );
  send_mqtt_string(client, "config/rootTopic", mqtt_topic_prefix, false );
  send_mqtt_string(client, "config/subsTopic", mqtt_subs_topic, false );
  send_mqtt_int(client, "config/timeZone", timeZone, false );

  if (hw_version ==  HW_AQM)
  {

  } else if (hw_version ==  HW_OCC) //If Occupancy Counter
  {

  send_mqtt_int(client, "config/scanZone", scan_zone, false );
  send_mqtt_int(client, "config/scanTime", blue_scan_time, false );
  send_mqtt_int(client, "config/maxDistance", blue_distance_max, false );
  send_mqtt_int(client, "config/minDistance", blue_distance_min, false );
  send_mqtt_int(client, "config/scanInterval", blue_interval, false );
  send_mqtt_int(client, "config/scanWindow", blue_window, false);
  send_mqtt_int(client, "config/calibrationRssi", calibrationRssi, false );

  }
  



  /*
    bool send_mqtt = DEFAULT_SEND_MQTT;
    byte scanning_frequency = DEFAULT_SCANNING_FREQUENCY;
    int blue_distance_max = DEFAULT_BLUE_RSSI_LIMIT_HIGH;
    int blue_scan_time = DEFAULT_BLUE_SCANTIME;
    int blue_interval = DEFAULT_BLUE_INTERVAL;
    int blue_window = DEFAULT_BLUE_WINDOW;
    int timeZone = 0000;

  */

}
