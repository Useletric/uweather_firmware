#ifndef _RTOS_H_
#define _RTOS_H_

#include "mqtt.h"
#include "sensor.h"
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <freertos/queue.h>

void rtosInit();
void windvelocity();
void RPMCalc();
void WindSpeed();
void SpeedWind(); 
void addcount();
void printMSG();

#endif