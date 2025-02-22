#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "sdfile.h"

#define SEALEVELPRESSURE_HPA (1013.25)

#define ANEMO_PIN 33 //
#define RAIN_SENSOR_PIN 27 //
#define BIRUT_PIN 32 //
#define VOLT_PIN 36
#define VOLT_BAT 35
#define AMOSTRAS 12
void initBME280();
void getDataBME280();
void windvelocity();
void RPMCalc();
void WindSpeed();
void SpeedWind();
void readWindDirection(int &wind_dir, String &direction); 
void IRAM_ATTR rainTrigger();
void addcount();
float voltageInput(uint8_t portaAnalogica);
void ShowData();
void readSensors();
#endif