/*
Air QUALITY SENSOR PM5005 and Grive Air Quality Sensor

Connect to PMS5003
 ____________________
|                    |
|                    |
|          12345678  |
|__________||||||||__|
            
8 -> 5V
7 -> GND
6 _> DNC
5 -> 17 ( Serial1 TX)
4 -> 16 ( Serial1 RX )


Connect the Grove Sensor MP503

VCC -> 3.3V on ESP32
GND -> GND on ESP32
SIG -> PIN 35 ( Analog In) on ESP32

*/

#include <Arduino.h>
#include <movingAvg.h> // https://github.com/JChristensen/movingAvg
#include <Adafruit_BME680.h>

#define PM5005_DATA_LENG 31 //Used for reading Serial data for Air PPM Sensor
unsigned char buf[PM5005_DATA_LENG];
#define MOVING_AVERAGE_SAMPLES 10

const uint8_t VOC_PIN(35);
movingAvg ma_voc(MOVING_AVERAGE_SAMPLES);   // define the moving average object
movingAvg ma_PM1_0(MOVING_AVERAGE_SAMPLES); // define the moving average object
movingAvg ma_PM2_5(MOVING_AVERAGE_SAMPLES); // define the moving average object
movingAvg ma_PM10(MOVING_AVERAGE_SAMPLES);  // define the moving average object

movingAvg ma_temp(MOVING_AVERAGE_SAMPLES);  // define the moving average object
movingAvg ma_pres(MOVING_AVERAGE_SAMPLES);  // define the moving average object
movingAvg ma_humi(MOVING_AVERAGE_SAMPLES);  // define the moving average object
movingAvg ma_alti(MOVING_AVERAGE_SAMPLES);  // define the moving average object
movingAvg ma_resis(MOVING_AVERAGE_SAMPLES); // define the moving average object

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

bool is_bme680_connected = false;

//Initialize teh sensors
void init_sensors()
{
    pinMode(VOC_PIN, INPUT_PULLUP);
    ma_voc.begin();
    ma_PM1_0.begin();
    ma_PM2_5.begin();
    ma_PM10.begin();

    if (bme.begin())
    {
        is_bme680_connected = true;
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320*C for 150 m
    }
    else
    {
        Serial.println("BME680 initialization Error");
    }
}

// used by Air PPM monitor
char checkValue(unsigned char *thebuf, char leng)
{
    char receiveflag = 0;
    int receiveSum = 0;

    for (int i = 0; i < (leng - 2); i++)
    {
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

bool read_pm_sensor(int &pm01, int &pm25, int &pm10)
{
    if (Serial1.find(0x42))
    { //start to read when detect 0x42
        Serial1.readBytes(buf, PM5005_DATA_LENG);

        if (buf[0] == 0x4d)
        {
            if (checkValue(buf, PM5005_DATA_LENG))
            {
                pm01 = ma_PM1_0.reading(transmitPM01(buf));  //count PM1.0 value of the air detector module
                pm25 = ma_PM2_5.reading(transmitPM2_5(buf)); //count PM2.5 value of the air detector module
                pm10 = ma_PM1_0.reading(transmitPM10(buf));  //count PM10 value of the air detector module
            }
        }
        return true;
    }
    else
        return false;
}

bool read_voc_sensor(int &ivoc)
{
    int reading = map(analogRead(VOC_PIN), 0, 1024, 0, 100); // read the Sensro
    ivoc = ma_voc.reading(reading);                          // calculate the moving average
    return true;
}

bool read_bme680_sensor(float &temperature, float &pressure, float &humidity, float &gas_resistance, float &altitude)
{
    if (is_bme680_connected)
    {
        if (bme.performReading())
        {
            Serial.print(bme.temperature);
            Serial.print(bme.pressure / 100.0);
            Serial.print(bme.humidity);
            Serial.print(bme.gas_resistance / 1000.0);
            Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
            return true;
        }
        else
            return false;
    }
    else
        return false;
}
/*
    if (_currentVoltage - _lastVoltage > 400 || _currentVoltage > 700) {
        return AirQualitySensor::FORCE_SIGNAL;
    } else if ((_currentVoltage - _lastVoltage > 400 && _currentVoltage < 700)
               || _currentVoltage - _standardVoltage > 150) {
        return AirQualitySensor::HIGH_POLLUTION;
    } else if ((_currentVoltage - _lastVoltage > 200 && _currentVoltage < 700)
               || _currentVoltage - _standardVoltage > 50) {
        return AirQualitySensor::LOW_POLLUTION;
    } else {
        return AirQualitySensor::FRESH_AIR;
    }
*/
