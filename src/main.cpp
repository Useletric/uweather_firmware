#include "main.h"

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN,INPUT);
  initBME280();
  mqttInit();     //iniica comunicação com MQTT
  rtosInit();     //inicialização das tasks e estrutuas do RTOS
}

void loop() {
  vTaskDelete(NULL);  //deleta task loop
}