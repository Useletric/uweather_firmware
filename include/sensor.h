#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>
#define SEALEVELPRESSURE_HPA (1013.25)

#define SENSOR_PIN 32

void initBME280();
float tempBME();
float umiBME();
float presureBME();
float aproxAltBME();
#endif