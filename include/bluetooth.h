#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <Arduino.h>
#include "BluetoothSerial.h"

// Inicializa o Bluetooth
void bluetoothInit(const char* deviceName = "ESP32_BT");

// Deve ser chamado no loop para verificar comandos recebidos
void bluetoothLoop();

// Variável global de controle (ex: último comando recebido)
extern String lastBtCommand;

#endif
