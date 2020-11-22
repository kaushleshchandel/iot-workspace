#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//Initialize the BLE Devices

//States of the devices
#define STATE_NEW  0
#define STATE_SAME  1
#define STATE_UPDATED  2
#define STATE_GONE -1


int device_count = 0;
int devices_added_count = 0;
int devices_removed_count = 0;

int new_devices_count = 0;
int OLD_device_count = 0;

int added_devices = 0;
int removed_devices = 0;

int occupant_count = 0;
int occupant_added_count = 0;
int occupant_removed_count = 0;
int occupant_moved_count = 0;

int prev_occupant_count = 0;
const int ARRAYSIZE = 50;


//Arrays to hold the data
String occupants[ARRAYSIZE];
String occupant_name[ARRAYSIZE+1];
double occupant_rssi[ARRAYSIZE+1];
int rssi_changed[ARRAYSIZE+1];
int rssi_exclude[ARRAYSIZE+1];

String OLD_occupants[ARRAYSIZE+1];
String OLD_occupant_name[ARRAYSIZE+1];
double OLD_occupant_rssi[ARRAYSIZE+1];
int OLD_rssi_changed[ARRAYSIZE+1];
int OLD_rssi_exclude[ARRAYSIZE+1];


char MAC_data[17];

int RSSI_data;

//For calibration

 String nearest_device_mac = "" ; // String to store Occupants Mac address
 int nearest_device_rssi = 0;
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {

    }
};

void initBLE(int interval, int window)
{
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(interval);
  pBLEScan->setWindow(window);  // less or equal setInterval value
}


double rssi_to_feet(int rssi )
{
  double dist;
  dist = pow(10.0, (double)(calibrationRssi - rssi) / 20.0); 
  return dist*3.28084;
}

void updatePrevDataArray()
{

  //Clean the OLD Array
  for (uint32_t i = 0; i < ARRAYSIZE; i++)
  {
    OLD_occupants[i] = "";
    OLD_occupant_rssi[i] = 0;
    OLD_occupant_name[i] = "";
    OLD_rssi_changed[ARRAYSIZE] = 0;
    OLD_rssi_exclude[ARRAYSIZE] = 0;
  }

  //Now fill the new array to the old one
  for (uint32_t i = 0; i < device_count; i++)
  {
    OLD_occupants[i] = occupants[i];
    OLD_occupant_rssi[i] = occupant_rssi[i];
    OLD_occupant_name[i] = occupant_name[i];
    OLD_rssi_changed[i] = rssi_changed[i];
    OLD_rssi_exclude[i] = rssi_exclude[i];
  }

}

void apply_rssi_thresholds()
{
  for ( int i = 0; i < device_count; i++ )
  {
    if (occupant_rssi[i] > blue_distance_max)
      rssi_exclude[i] = 1;
    else if (occupant_rssi[i] < blue_distance_min)
      rssi_exclude[i] = -1;
    else
      rssi_exclude[i] = 0;
  }

}

bool checkDeviceHistory()
{

  added_devices = 0;
  removed_devices = 0;

  if (OLD_device_count > 0)
  {

    int i = 0;
    int j = 0;

    //Check which device has moved
    for ( i = 0; i < device_count; i++ )
    {
      rssi_changed[i] = STATE_NEW;

      for (j = 0; j < OLD_device_count; j++)
      {
        if (occupants[i] == OLD_occupants[j] )
        {
          rssi_changed[i] = STATE_SAME;
          // Serial.println("New : Same");

          /*
            #define STATE_NEW  0
            #define STATE_SAME  1
            #define STATE_UPDATED  2
            #define STATE_GONE -1
          */
        }

      }
    }


    i = 0;
    j = 0;

    //Check which device has moved
    for ( i = 0; i < OLD_device_count; i++ )
    {
      OLD_rssi_changed[i] = STATE_GONE;
      for (j = 0; j < device_count; j++)
      {
        if (OLD_occupants[i] == occupants[j] )
        {
          OLD_rssi_changed[i] = STATE_SAME;
        }
      }
    }

    //Now count whas is new & what is gone

    for (int k = 0; k < device_count; k++)
    {
      if (rssi_changed[k] == STATE_NEW)
        added_devices++;
    }

    for (int l = 0; l < OLD_device_count; l++)
    {
      if (OLD_rssi_changed[l] == STATE_GONE)
        removed_devices++;
    }

  }

  Serial.printf(" %d + %d - %d ] ", OLD_device_count, added_devices, removed_devices);
 return true;
}

int getNewOccupants()
{

  occupant_count = 0;

  Serial.print("SCAN: ");

  BLEDevice::init("");
  pBLEScan->setInterval(blue_interval);
   
  BLEScanResults foundDevices = pBLEScan->start(blue_scan_time, false);

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

  device_count = foundDevices.getCount();
  // Serial.printf("D: %d", device_count);

  String s_mac_address ; // String to store Occupants Mac address
  String s_name ; // String to store Occupants Name

  //Clean the Array
  for (uint32_t i = 0; i < ARRAYSIZE; i++)
  {
    occupants[i] = "";
    occupant_rssi[i] = 0;
    rssi_changed[i] = 0;
  }


  //Scan and add the new devices to the list
  for (uint32_t i = 0; i < device_count; i++)
  {

    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    RSSI_data = device.getRSSI();
    s_name = device.getName().c_str();

    strcpy( MAC_data, device.getAddress().toString().c_str());
    s_mac_address = MAC_data;
    s_mac_address.replace(":", "");
    occupants[i] = s_mac_address;
    occupant_rssi[i] = rssi_to_feet(RSSI_data);
    occupant_count += 1;
  }

  Serial.printf(" [ %d = ", device_count);

  apply_rssi_thresholds();
  checkDeviceHistory();
  updatePrevDataArray();

  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
 

  return occupant_count;
}
