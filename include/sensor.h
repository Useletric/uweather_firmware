#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define SENSOR_PIN 32
#define VOLT_PIN 36
#define VOLT_BAT 35
#define AMOSTRAS 12
void initBME280();
void getDataBME280();
void windvelocity();
void RPMCalc();
void WindSpeed();
void SpeedWind(); 
void addcount();
float voltageInput(uint8_t portaAnalogica);
void ShowData();
#endif