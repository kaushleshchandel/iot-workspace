#include <Arduino.h>

#define PM5005_DATA_LENG 31   //Used for reading Serial data for Air PPM Sensor
unsigned char buf[PM5005_DATA_LENG];


// used by Air PPM monitor 
char checkValue(unsigned char *thebuf, char leng)
{
  char receiveflag = 0;
  int receiveSum = 0;

  for (int i = 0; i < (leng - 2); i++) {
    receiveSum = receiveSum + thebuf[i];
  }
  receiveSum = receiveSum + 0x42;

  if (receiveSum == ((thebuf[leng - 2] << 8) + thebuf[leng - 1])) //check the serial data
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}


// used by Air PPM monitor 
int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val = ((thebuf[3] << 8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

//transmit PM Value to PC
int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val = ((thebuf[5] << 8) + thebuf[6]); //count PM2.5 value of the air detector module
  return PM2_5Val;
}

//transmit PM Value to PC
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[7] << 8) + thebuf[8]); //count PM10 value of the air detector module
  return PM10Val;
}


bool read_pm_sensor(int& pm01, int& pm25, int& pm10)
{
  if (Serial.find(0x42)) {  //start to read when detect 0x42
    Serial.readBytes(buf, PM5005_DATA_LENG);

    if (buf[0] == 0x4d) {
      if (checkValue(buf, PM5005_DATA_LENG)) {
        pm01 = transmitPM01(buf); //count PM1.0 value of the air detector module
        pm25 = transmitPM2_5(buf); //count PM2.5 value of the air detector module
        pm10 = transmitPM10(buf); //count PM10 value of the air detector module
      }
    }
    return true;
  } else return false;

}
