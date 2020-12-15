#ifndef BLE_COUNTER_H /* include guards */
#define BLE_COUNTER_H
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>

#define DEFAULT_ARRAYSIZE 100

class BLE_counter
{
public:
  BLE_counter();
  void get_count(int &deviceTotal, int &peopleTotal);
  void init(bool activeScan, int interval, int window, int maxDistance, int minDistance, int scanTime, int scanWwindow, int calrssi);
  void set_parameters(bool activeScan, int interval, int window, int maxDistance, int minDistance, int scanTime, int scanWwindow, int calrssi);

private:
  int _activeScan;
  int _maxDistance;
  int _minDistance;
  int _interval;
  int _window;
  int _scantime;
  int _calibrationRssi = -75;
  const int _arrayasize = DEFAULT_ARRAYSIZE;
  double _occupant_rssi[DEFAULT_ARRAYSIZE + 1];

  BLEScan *pBLEScan;
  int rssi_to_feet(int rssi);
};

#endif