#ifndef SDFILE_H
#define SDFILE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SCK  14
#define MISO  2
#define MOSI  15
#define CS  13

void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);

void debug_sd();
void init_sd();
void store_sd(const char *nomeArquivo, const JsonDocument& jsonDoc);
void getID();
void loadConfiguration(const char *filename);
#endif