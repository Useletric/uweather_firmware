#ifndef _RTOS_H_
#define _RTOS_H_

#include "mqtt.h"
#include "sensor.h"

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include "freertos/event_groups.h"

void rtosInit();



#endif