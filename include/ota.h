#ifndef _OTA_H_
#define _OTA_H_

#include <Arduino.h>

#include <otadrive_esp.h>
#include <SPIFFS.h>

#define APIKEY "8082a29d-6b57-4e85-b391-f0aae076343f"
void ota_prep();
void fn_update();
void update();
void onUpdateProgress(int progress, int totalt);
void listDirSPIFFS(fs::FS &fs, const char *dirname, uint8_t levels);

#endif