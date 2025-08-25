#include "bluetooth.h"

BluetoothSerial SerialBT;
String lastBtCommand = "";

void bluetoothInit(const char* deviceName) {
    if (!SerialBT.begin(deviceName)) {
        Serial.println("Erro ao iniciar Bluetooth!");
    } else {
        Serial.print("Bluetooth iniciado como: ");
        Serial.println(deviceName);
    }
}

void bluetoothLoop() {
    if (SerialBT.available()) {
        // Lê a mensagem recebida
        String command = SerialBT.readStringUntil('\n');
        command.trim();  // remove espaços e quebras de linha
        lastBtCommand = command;

        Serial.print("Comando recebido via Bluetooth: ");
        Serial.println(command);

        // Se o comando for RESTART → reinicia o ESP32
        if (command.equalsIgnoreCase("ORDER66")) {
            SerialBT.println("♻ Reiniciando ESP32 via Bluetooth...");
            Serial.println("♻ Reiniciando ESP32 via Bluetooth...");
            delay(500);
            ESP.restart();
        }
    }
}
