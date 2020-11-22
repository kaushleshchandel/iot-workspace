#ifndef BLE_COUNTER_H /* include guards */
#define BLE_COUNTER_H
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>


class BLE_counter
{
  public:
    BLE_counter();
    void get_count(int &deviceTotal, int &peopleTotal );
    void init(int interval, int window, int maxDistance, int minDistance, int scanTime, int scanWwindow);    
  private:
    int _maxDistance;
    int _minDistance;
    int _interval;
    int _window;
    int _scantime;
    int _calibrationRssi = -75;
    const int _ARRAYSIZE = 100;
    double _occupant_rssi[101];

    BLEScan* pBLEScan;
    int rssi_to_feet(int rssi);

};


#endif