#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "sdfile.h"
#include "driver/pcnt.h"

#define RAIN_PCNT_UNIT   PCNT_UNIT_0
#define RAIN_PCNT_CH     PCNT_CHANNEL_0
#define PCNT_H_LIM_VAL   10000
#define RAIN_MM_POR_PULSO 0.25f

#define SEALEVELPRESSURE_HPA (1013.25)

#define ANEMO_PIN 33 //27
#define RAIN_SENSOR_PIN 27 //33
#define BIRUT_PIN 32 //
#define VOLT_PIN 36
#define VOLT_BAT 35
#define AMOSTRAS 12

void initRainPCNT();
void resetRainDaily();
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