#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <TimeLib.h>
#include "structs.h"
String getCurrentDateTime();
void setClock();
#endif