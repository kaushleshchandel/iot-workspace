#include "ble_counter.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>



BLE_counter::BLE_counter()
{
    
}

void BLE_counter::init(int interval, int window, int maxDistance, int minDistance, int scanTime, int scanWwindow)
{
 // pinMode(pin, 1);
 
 _interval = interval;
 _maxDistance = maxDistance;
 _minDistance = minDistance;
 _scantime = scanTime;
 _window = scanWwindow;

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(_interval);
  pBLEScan->setWindow(_window);  // less or equal setInterval value

}
void BLE_counter::get_count(int &deviceTotal, int &peopleTotal )
{
   
    peopleTotal = 0;
  Serial.print("SCAN: ");

  BLEDevice::init("");
  pBLEScan->setInterval(_interval);
   
  BLEScanResults foundDevices = pBLEScan->start(_scantime, false);

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

  deviceTotal = foundDevices.getCount();

  for (int i = 0; i < deviceTotal; i++)
  {

    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    int distance = rssi_to_feet(device.getRSSI());
    Serial.print(distance);    
  //  Serial.print(":");

    if( distance < _maxDistance && distance >> _minDistance  )
    {
        peopleTotal ++;
        Serial.print(":Y | ");
    } else
    {
          Serial.print(":N | ");
    }
    
        
  }
Serial.println(" ");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  
}

int BLE_counter::rssi_to_feet(int rssi)
{
double dist;
  dist = pow(10.0, (double)(_calibrationRssi - rssi) / 20.0); 
  return dist*3.28084;
}