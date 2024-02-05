#ifndef _OTA_H_
#define _OTA_H_

#include <Arduino.h>

#include <otadrive_esp.h>
#include <SPIFFS.h>

#define APIKEY "c9122130-9687-49b6-a9a2-082297e0d755"
void ota_prep();
void fn_update();
void update();
void onUpdateProgress(int progress, int totalt);
void listDirSPIFFS(fs::FS &fs, const char *dirname, uint8_t levels);

#endif