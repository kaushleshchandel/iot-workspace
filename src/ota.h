#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <Update.h>

#include <HTTPClient.h>
#define FIRMWARE_SERVER "http://34.66.141.185/abeacon/update_" 

/*
Update the firmware to provided file as sURL
*/

void updateFirmware(String sURL)
{
  if ((WiFi.status() == WL_CONNECTED))
  {
    //Check if the Firmware file exists
    Serial.print("Checking for ");
    Serial.println(sURL);

    t_httpUpdate_return ret = ESPhttpUpdate.update(sURL);
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}

/*
Checks for the firmware based on Software / Hardware version
Automatically check for an update file on the server

*/

bool checkUpdateFirmware(String currentFirmware, String HardwareVersion )
{
  String sURL = FIRMWARE_SERVER;
  sURL += HardwareVersion;
  sURL += "_";
  sURL += currentFirmware;
  sURL += ".bin";

  boolean isUpdaetAvailable = false;

  Serial.println("Checking for new firmware");
  Serial.println(sURL);

  HTTPClient http;

  http.begin(sURL); //HTTP
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK)
    {
      isUpdaetAvailable =  true;
      Serial.println("Updating firmware");
      updateFirmware(sURL);
    }
  } else {
    Serial.println("No Firmware update");
    isUpdaetAvailable = false;
  }

  http.end();

  return isUpdaetAvailable;
}

