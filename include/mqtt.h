#ifndef _MQTT_H_
#define _MQTT_H_

#include <Arduino.h>

/*bibliotecas para MQTT*/
#include <WiFi.h>
#include <PubSubClient.h>
#include "rtc.h"
#include "sdfile.h"

void mqttInit();
void mqttIsConected();
void mqttSend(char mensagem[]);
void setClock();
void mqttDisconnect();
void streamingData();
bool shouldEnterDeepSleep();
void enterDeepSleep();
bool shouldTransmit();
#endif